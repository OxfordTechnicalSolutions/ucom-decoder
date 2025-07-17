import socket

class UDPSocketReader:
    def __init__(self, filter_ip = None, port = 50487):
        self.socket_port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        try:
            self.sock.bind(('', self.socket_port))
            self.valid = True
            self.socket_status = "OK"
        except socket.error as message:
            self.valid = False
            self.socket_status = message

        self.filter_ip = filter_ip if filter_ip is not None else "" 
        self.ip_filter_active = filter_ip is not None

    def __del__(self):
        try:
            self.sock.close()
        except:
            pass

    def read(self):
        """Read UDP packet
         
            Returns: a bytes object, or None if no packet available
        """
        nb, addrport = self.sock.recvfrom(4096) # New bytes
        
        source_addr, source_port = addrport
        if  (self.ip_filter_active and source_addr != self.filter_ip) or len(nb) == 0:
            return None
        else:
            return nb
