from dataclasses  import dataclass
from typing import ClassVar
import struct
from ucomgen.crc import crc32
from enum import Enum

class GnssOffset:
    def __init__(self) -> None:
        self.is_available: bool = False
        self.value: int = 0

@dataclass
class UcomTestSignal:
    id : str
    data_type : str

    def generate_value(self, value : int, enum_member : int = -1):
        if self.data_type == 'U8':
            return struct.unpack('B', self.__generate_bytearray(1, value))[0]
        elif self.data_type == 'U8':
            return struct.unpack('B', self.__generate_bytearray(1, value))[0]
        elif self.data_type == 'S16':
            return struct.unpack('<h', self.__generate_bytearray(2, value))[0]
        elif self.data_type == 'U16':
            return struct.unpack('<H', self.__generate_bytearray(2, value))[0]
        elif self.data_type == 'F64':
            return struct.unpack('d', self.__generate_bytearray(8, value))[0]
        elif self.data_type == 'S32':
            return struct.unpack('<i', self.__generate_bytearray(4, value))[0]
        elif self.data_type == 'U32':
            return struct.unpack('<I', self.__generate_bytearray(4, value))[0]
        elif self.data_type == 'S64':
            return struct.unpack('<q', self.__generate_bytearray(8, value))[0]
        elif self.data_type == 'U64':
            return struct.unpack('<Q', self.__generate_bytearray(8, value))[0]
        elif self.data_type == 'F32':
            return struct.unpack('<f', self.__generate_bytearray(4, value))[0]
        elif self.data_type == 'F64':
            return struct.unpack('<d', self.__generate_bytearray(8, value))[0]
        elif self.data_type == 'EnS64':
            # EnS64: struct with uint8_t and int64_t
            ba = self.__generate_bytearray(9, value)
            return struct.unpack('<Bq', ba)
        elif self.data_type == 'STR':
            text_bytes = f"Test string {value}\0".encode('utf-8')
            packed = struct.pack(f'{len(text_bytes)}s', text_bytes)
            return struct.unpack(f'{len(text_bytes)}s', packed)
        else:
            raise ValueError(f"Unsupported data type: {self.data_type}")
               
    def generate_data(self, value : int, enum_member : int = -1) -> bytearray:
        """Generates signal data based on 'value'
        
        Returns: bytearray
        """
        if self.data_type == 'U8':
            return self.__generate_bytearray(1, value)
        elif self.data_type == 'U8':
            return self.__generate_bytearray(1, value)
        elif self.data_type == 'S16':
            return self.__generate_bytearray(2, value)
        elif self.data_type == 'U16':
            return self.__generate_bytearray(2, value)
        elif self.data_type == 'F64':
            return self.__generate_bytearray(8, value)
        elif self.data_type == 'S32':
            return self.__generate_bytearray(4, value)
        elif self.data_type == 'U32':
            return self.__generate_bytearray(4, value)
        elif self.data_type == 'S64':
            return self.__generate_bytearray(8, value)
        elif self.data_type == 'U64':
            return self.__generate_bytearray(8, value)
        elif self.data_type == 'F32':
            return self.__generate_bytearray(4, value)
        elif self.data_type == 'F64':
            return self.__generate_bytearray(8, value)
        elif self.data_type == 'EnS64':
            # EnS64: struct with uint8_t and int64_t
            if enum_member == -1:
                return self.__generate_bytearray(9, value)
            else:
                enum_byte = bytearray(1)
                enum_byte[0] = enum_member
                return enum_byte + self.__generate_bytearray(8, value)
        elif self.data_type == 'STR':
            text_bytes = f"Test string {value}".encode('ascii')
            buffer = bytearray(len(text_bytes) + 1)
            buffer[:len(text_bytes)] = text_bytes
            #packed = struct.pack(f'{len(text_bytes)}s', text_bytes)
            return buffer
        else:
            raise ValueError(f"Unsupported data type: {self.data_type}")
        
    def get_value(self, data : bytearray):
        if self.data_type == 'U8':
            return struct.unpack('B', data)[0]
        elif self.data_type == 'S8':
            return struct.unpack('b', data)[0]
        elif self.data_type == 'S16':
            return struct.unpack('<h', data)[0]
        elif self.data_type == 'U16':
            return struct.unpack('<H', data)[0]
        elif self.data_type == 'F64':
            return struct.unpack('d', data)[0]
        elif self.data_type == 'S32':
            return struct.unpack('<i', data)[0]
        elif self.data_type == 'U32':
            return struct.unpack('<I', data)[0]
        elif self.data_type == 'S64':
            return struct.unpack('<q', data)[0]
        elif self.data_type == 'U64':
            return struct.unpack('<Q', data)[0]
        elif self.data_type == 'F32':
            return struct.unpack('<f', data)[0]
        elif self.data_type == 'F64':
            return struct.unpack('<d', data)[0]
        elif self.data_type == 'EnS64':
            # EnS64: struct with uint8_t and int64_t
            values = struct.unpack('<Bq', data)
            return  values[0],values[1]
        elif self.data_type == 'STR':
            return data[:-1].decode(encoding='ascii')
        else:
            raise ValueError(f"Unsupported data type: {self.data_type}")
        
    def __generate_bytearray(self, size : int, value : int):
        """Returns a bytearray of size 'size' containing bytes set to 'value'"""
        if value > 255:
            raise ValueError(f"Value ({value}) is greater than 255")
        data = bytearray(size)
        for index in range(0, len(data)):
            data[index] = value
        return data

class OutputTypes(Enum):
    FrequencyBased = 0
    OnTrigger = 1
    OnChange = 2

class TriggerTypes(Enum):
    NO_TRIGGER = 0
    IN_1_DOWN = 1
    IN_1_UP = 2
    OUT_1 = 3
    IN_2_DOWN = 4
    IN_2_UP = 5
    UNKNOWN_1 = 6
    UNKNOWN_2 = 7
    OUT_2 = 8

trigger_descriptions = dict([
        (TriggerTypes.NO_TRIGGER , 'No Trigger'),
        (TriggerTypes.IN_1_DOWN , 'In 1 Down'),
        (TriggerTypes.IN_1_UP , 'In 1 Up'),
        (TriggerTypes.OUT_1 , 'Out 1'),
        (TriggerTypes.IN_2_DOWN , 'In 2 Down'),
        (TriggerTypes.IN_2_UP , 'In 2 Up'),
        (TriggerTypes.UNKNOWN_1 , 'Unknown 1'),
        (TriggerTypes.UNKNOWN_2 , 'Unknown 2'),
        (TriggerTypes.OUT_2 , 'Out 2')
    ])

class TimeFrames(Enum):
    SDN = 0
    GNSS = 1

class TimeSources(Enum):
    TIME_SOURCE_NONE = 0
    TIME_SOURCE_GNSS = 1
    TIME_SOURCE_PTP = 2
    TIME_SOURCE_EXT_GNSS = 3
    TIME_SOURCE_USER = 4
    TIME_SOURCE_GAD = 5
    TIME_SOURCE_UNKNOWN = 6

time_source_descriptions = dict([
    (TimeSources.TIME_SOURCE_NONE, 'None'),
    (TimeSources.TIME_SOURCE_GNSS, 'GNSS'),
    (TimeSources.TIME_SOURCE_PTP, 'PTP'),
    (TimeSources.TIME_SOURCE_EXT_GNSS, 'ExtGNSS'),
    (TimeSources.TIME_SOURCE_USER, 'User'),
    (TimeSources.TIME_SOURCE_GAD, 'GAD'),
    (TimeSources.TIME_SOURCE_UNKNOWN, 'Unknown')
])

@dataclass 
class UcomTestMessage:
    id : int
    version : int
    enabled : bool
    output_type : OutputTypes
    signals : list[UcomTestSignal]
    timing : str

    enum_member : ClassVar[int] = 1

    def generate_packet(self, time : TimeFrames, arbitrary_time : int, value : int, gnss_offset: GnssOffset) -> tuple[bytearray, str]:
        data = bytearray(1500)
        generated_value = None
        line : str = ''
        payload_length = 0
        info = time.value
        if self.output_type == OutputTypes.OnTrigger:
            info = info | (self.get_trigger_type().value << 4)
        
        offset = 16
        for signal in self.signals:
            signal_data : bytearray
            signal_data = signal.generate_data(value, UcomTestMessage.enum_member)
            generated_value = signal.get_value(signal_data)
            data[offset:] = signal_data
            offset = offset + len(signal_data)
            
            # Construct CSV
            if len(line) > 0:
                line = line + ','

            if signal.data_type == 'EnS64':
                UcomTestMessage.enum_member = UcomTestMessage.enum_member + 1
                if UcomTestMessage.enum_member > TimeSources.TIME_SOURCE_UNKNOWN.value:
                    UcomTestMessage.enum_member = 1
                line = line + time_source_descriptions[TimeSources(generated_value[0])] + ',' + str(generated_value[1])
            else:
                if signal.data_type == 'F32':
                    line = line + str(f"{generated_value:.9e}")
                elif signal.data_type == 'F64':
                    line = line + str(f"{generated_value:.15e}")
                else:
                    line = line + str(generated_value)

            if signal.id == 'SDNOFF' and TimeSources(generated_value[0]) == TimeSources.TIME_SOURCE_GNSS:
                # print(f'{signal.id} {TimeSources(generated_value[0])} {int(generated_value[1])}')
                gnss_offset.is_available = True
                gnss_offset.value = int(generated_value[1])
        
        if self.output_type == OutputTypes.OnTrigger:
            line = line + ',' + trigger_descriptions[self.get_trigger_type()]
        
        payload_length = offset - 16

        # Write header
        sync = 'UM'.encode('utf-8')
        struct.pack_into('<2sHBBqH', data, 0, sync, self.id, self.version, info, arbitrary_time, payload_length)
        crc = self.__calculate_crc(data)
        data = data + struct.pack('I', crc)
        return data, line
    
    def __calculate_crc(self, payload):
        return crc32(payload)
    
    def set_trigger_type(self, trigger : TriggerTypes):
        self.__trigger_type = trigger

    def get_trigger_type(self) -> TriggerTypes:
        try:
            return self.__trigger_type
        except:
            return TriggerTypes.NO_TRIGGER
    def uid(self):
        if self.version == 0:
            return f"{self.id:03d}"
        else:
            return f"{self.id:03d}v{self.version}"
    
    def header(self) -> str:
        """Returns a CSV header"""
        csv = ''
        for signal in self.signals:
            csv = csv + ',' + ('Enum element,' if signal.data_type == 'EnS64' else '') + signal.id
        if self.output_type == OutputTypes.OnTrigger:
            csv = csv + ',Trigger Type'
        return f"Time ({self.timing}),GNSS Time{csv}"
