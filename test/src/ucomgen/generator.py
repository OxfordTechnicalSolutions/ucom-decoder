import io, os
from ucomgen.ucom_definitions import UcomTestMessage, OutputTypes, TriggerTypes, TimeFrames, GnssOffset

class UcomGenerator:
    def __init__(self, output_path: str = 'test_data', filename : str = 'data.ucom') -> None:
        self.__output_path = output_path
        self.__filename = filename

    def generate_data(self, messages : dict[int, UcomTestMessage]):
        """Generates test UCOM data and saves it in <output_path>/<filename>

        Parameters:
            messages (dict[int, UcomTestMessage]): Dictionary of UcomTestMessages (from DBU file), keyed by ID
        """
        data = bytearray()
        os.makedirs('test_data', exist_ok=True)
        f = open('test_data/data.ucom', 'wb')
        csv_files : dict[str, io.TextIOWrapper] = {}
        offset = 0
        message : UcomTestMessage
        timestamp: int = 1000000000
        trigger = 1
        gnss_offset = GnssOffset()
        for i in range(0, 256):
            for _, message in messages.items():
                line = str(timestamp + i) + ',' 
                if message.enabled:
                    if message.uid() not in csv_files:
                        filename = f"test_data/data_{message.uid()}.csv"
                        csv_files[message.uid()] = open(filename, "wt")
                        csv_files[message.uid()].write(f'{message.header()}\n')
                    if message.output_type == OutputTypes.OnTrigger:
                        message.set_trigger_type(TriggerTypes(trigger))
                        trigger = trigger + 1
                        if trigger > TriggerTypes.OUT_2.value:
                            trigger = TriggerTypes.IN_1_DOWN.value
                    packet, data_str = message.generate_packet(TimeFrames.SDN, timestamp + i, i, gnss_offset)

                    line = line + (str(gnss_offset.value + timestamp + i) if gnss_offset.is_available else '') + ',' + data_str
                    data[offset:] = packet
                    offset = offset + len(packet)
                    if offset >= 64000:
                        f.write(data)
                        data = bytearray()
                        offset = 0
                    # print(packet)
                    csv_files[message.uid()].write(f'{line}\n')
        if offset > 0:
            f.write(data)
        f.close()
        for _, data_csv_file in csv_files.items():
            data_csv_file.close()