import unittest
import json
from oxts.ucompy import UcomSignal
import data as test_data

class UcomSignalTest(unittest.TestCase):
    def setUp(self) -> None:
        self.message_dict = test_data.message_dict

        self.message_str = json.dumps(self.message_dict)

        self.signal = UcomSignal(json.dumps(self.message_dict["SignalsInMessage"][0]))

        return super().setUp()
    
    def tearDown(self) -> None:
        return super().tearDown()
    
    def test_constructor(self):
        assert(type(self.signal) is UcomSignal)

    def test_get_signal_id(self):
        """
        Tests get_signal_id()
        """
        assert(self.signal.get_signal_id() == "UCOMVersion")
        signal = UcomSignal(json.dumps(self.message_dict["SignalsInMessage"][1]))
        assert(signal.get_signal_id() == "DevID")
        signal = UcomSignal(json.dumps(self.message_dict["SignalsInMessage"][2]))
        assert(signal.get_signal_id() == "Wd")

    def test_get_data_type(self):
        """
        Tests get_signal_id()
        """
        signal_data_type = self.signal.get_data_type()
        assert(signal_data_type.name == 'U32')
        signal = UcomSignal(json.dumps(self.message_dict["SignalsInMessage"][1]))
        signal_data_type = signal.get_data_type()
        assert(signal_data_type.name == 'STR')
        signal = UcomSignal(json.dumps(self.message_dict["SignalsInMessage"][2]))
        signal_data_type = signal.get_data_type()
        assert(signal_data_type.name == 'F64')

