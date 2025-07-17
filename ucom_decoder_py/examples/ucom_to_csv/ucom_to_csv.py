from oxts.ucompy import UcomData, UcomDbu, UcomSignal, UcomMessage
import os
import time
import argparse
import sys
from udp_socket import UDPSocketReader

class Interval:
    def __init__(self, duration = 0):
        self.interval_start = 0
        self.duration = duration

    def start(self):
        self.interval_start = time.time()

    def is_elapsed(self):
        return self.duration > 0 and (time.time() - self.interval_start >= self.duration)
    
    def active(self):
        return self.duration > 0 and not self.is_elapsed()

class UcomToCsv:
    def __init__(self, dbu_filename, message_ids = [], output_file_prefix = "output_"):
        self.output_files = {}
        self.dbu = UcomDbu(dbu_filename)
        self.valid = self.dbu.get_valid()
        self.output_file_prefix = output_file_prefix
        self.input = None
        self.packets = 0
        self.message_ids = message_ids if message_ids is not None else []
    
    def __repr__(self):
        return f"UcomToCsv: .dbu = {self.dbu.get_filename()}, valid = {self.valid}"

    def create_output_file(self, filename_prefix, message_id, header):
        filename = f"{filename_prefix}{message_id:03d}.csv"
        try:
            self.output_files[message_id] = open(filename, "w")
            return True
        except:
            pass
        return False

    def create_output_files(self):
        for id in self.dbu.get_message_ids():
            if not self.create_output_file(self.output_file_prefix, id, self.dbu.get_message(id).get_header()):
                return False
        return True

    def close_output_files(self):
        for file in self.output_files.values():
            try:
                file.close()
            except:
                print(f"Error closing output file {file.name}")

    def open_input_file(self, filename):
        try:
            self.input = open(filename, "rb")
        except:
            print("Failed to open input file filename")
            return False
        return True

    def file_size(self, file):
        """Try to get the size of a file
        
        Parameters:
            file -- a file object

        Returns: the file size, or -1 if fails 
        """
        try:
            file.seek(0, os.SEEK_END)
            size = file.tell()
            file.seek(0)
            return size
        except:
            return -1
        
    def write_packet_csv(self, packet):
        if packet.get_message_id() in self.output_files:
            self.write_csv(self.output_files[packet.get_message_id()], packet.get_csv())
    
    def write_csv(self, file, csv):
        file.write(f"{csv}\n")
        
    def process_file(self, filename):
        """Decodes a .ucom input file
        
        Parameters:
            filename -- the input file name

        Returns: True if succeeds, False otherwise
        """

        if not self.create_output_files():
            print("Failed to create output files")
            return False

        if not self.open_input_file(filename):
            return False
        
        left = self.file_size(self.input)
        
        offset = 0
        self.packets = 0
        time_start = time.time()
        # Read the input file in chunks until the end
        while left > 0:
            start = 0
            # Adjust the file pointer if required (see need_more_data, below)
            self.input.seek(offset, os.SEEK_CUR)
            chunk = self.input.read(2048)
            end = len(chunk)
            while start < end:
                need_more_data = False
                nibble = chunk[start:]
                # Check for a candidate packet at 'start'
                length, need_more_data = UcomData.peek(nibble, len(nibble))
                if length >= 20:
                    # Found a candidate; try to decode the full packet
                    d = UcomData(nibble, length, self.dbu)
                    if d.get_valid():
                        # Packet is valid, write it to file (if in list of required message IDs)
                        if len(self.message_ids) == 0 or d.get_message_id() in self.message_ids:
                            self.write_packet_csv(d)
                            self.packets = self.packets + 1
                        # Adjust the position to start searching for the next packet
                        start = start + length
                        left = left - length                
                    else:
                        start = start + 1
                else:
                    if need_more_data:
                        # Start of candidate packet found, but need more data
                        # Calculate the new file pointer offset
                        offset = start - end
                        break
                    else:
                        # No candidate found, step to next byte
                        start = start + 1

        print(f"Time taken: {time.time() - time_start} seconds")
        self.close_output_files()
        return True

    def process_udp(self, filter_ip = None, max_packets = 0, duration = 0):
        """Decodes a .ucom UDP stream
        
        Parameters:
            filter_ip -- only decode packets from this IP address
            max_packets -- maximum number of packets to capture
            duration -- maximum length of capture (in seconds)
        Returns: True if succeeds, False otherwise
        """
        if not self.create_output_files():
            print("Failed to create output files")
            return False

        # Create a socket to read UDP packets from the desired IP address
        udp = UDPSocketReader(filter_ip)
        if not udp.valid:
            print(f"Error creating UDP reader: {udp.socket_status}")
            return False
        timer = Interval(duration)
        timer.start()
        while self.packets < max_packets or (timer.active() and not timer.is_elapsed()):
            data = udp.read()
            if data is not None:
                d = UcomData(data, len(data), self.dbu)
                if d.get_valid():
                    # Packet is valid, write it to file (if in list of required message IDs)
                    if len(self.message_ids) == 0 or d.get_message_id() in self.message_ids:
                        self.write_packet_csv(d)
                        self.packets = self.packets + 1

        if timer.is_elapsed() and (max_packets == 0 or self.packets < max_packets):
            print("Maximum capture duration reached") 

        self.close_output_files()

class Arguments:
    def __init__(self):
        self.parser = argparse.ArgumentParser("UcomToCsv")
        self.parser.add_argument('-u', '--dbu-file', required=True, help='path to the .dbu file')
        self.parser.add_argument('-f', '--input-file', help='Extract data from a file instead of UDP stream')
        self.parser.add_argument('-m', '--message-ids', help='<id1 id2 id3 ...>   Message IDs to process, e.g. 0 1 3')
        self.parser.add_argument('-c', '--max-packets', help='Number of packets to process (UDP stream only)', default=0)
        self.parser.add_argument('-t', '--duration', help='Maximum capture duration in seconds (UDP stream only)', type=int, default=0)
        self.parser.add_argument('-i', '--source-ip', help='Source IP address (UDP stream only)', default='')
        self.parser.add_argument('-o', '--output-file', help='Output file prefix (default is output_)', default='output_')
        self.parser.add_argument('-a', '--disable-user-abort', help='Disable user-abort', default=False)
        self.args = None
        try:
            self.args = self.parser.parse_args()
        except:
            sys.exit()

if __name__ == "__main__":
    args = Arguments().args
    message_ids = [int(x) for x in args.message_ids.split()] if args.message_ids is not None else []

    app = UcomToCsv(args.dbu_file, message_ids=message_ids, output_file_prefix=args.output_file)
    print(app)
    if args.input_file is None:
        app.process_udp(filter_ip=args.source_ip, max_packets=args.max_packets, duration=args.duration)
    else:
        app.process_file(args.input_file)
    
    print(f"Packets decoded and written to file: {app.packets}")


