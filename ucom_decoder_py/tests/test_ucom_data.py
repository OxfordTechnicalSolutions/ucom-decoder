import unittest
import json
import ucomgen.generator as g
from ucomgen.ucom_definitions import UcomTestMessage, UcomTestSignal, OutputTypes, TimeSources, GnssOffset, TimeFrames
from oxts.ucompy import UcomMessage, UcomSignal, UcomDbu, UcomData
import data as test_data

class UcomGeneratorTest(unittest.TestCase):
    def setUp(self) -> None:
        self.message_str = json.dumps(test_data.message_dict)

        self.dbu = UcomDbu('mobile.dbu')

        self.message = UcomMessage(self.message_str)
        # Create a list of signals 
        self.signals = [UcomTestSignal('UCOMVersion','U32'), UcomTestSignal('DevID','STR'), UcomTestSignal('Wd','F64')]
        # 'id', 'version', 'enabled', 'output_type', 'signals', and 'timing'
        self.test_message = UcomTestMessage(64512, 0, True, OutputTypes.OnTrigger, self.signals, TimeSources.TIME_SOURCE_GNSS)
        self.packet, c = self.test_message.generate_packet(TimeFrames.SDN, 999999999, 120, GnssOffset())
        self.data = UcomData(self.packet, len(self.packet), self.dbu)

        return super().setUp()

    def tearDown(self) -> None:
        return super().tearDown()
    
    def test_get_valid(self) -> None:
        """
        Tests get_valid()
        """
        assert(self.data.get_valid() == True)

    def test_get_message_id(self) -> None:
        """
        Tests get_message_id()
        """
        assert(self.data.get_message_id() == 64512)

    def test_get_message_version(self) -> None:
        """
        Tests get_message_version()
        """
        assert(self.data.get_message_version() == 0)

    def test_get_message_uid(self) -> None:
        """
        Tests get_message_uid()
        """
        assert(self.data.get_message_uid() == 64512)

    def test_get_time_frame(self) -> None:
        """
        Tests get_time_Frame()
        """ 
        assert(self.data.get_time_frame() == TimeFrames.SDN.value)

    def test_get_arbitrary_time(self) -> None:
        """
        Tests get_arbitrary_time()
        """

        assert(self.data.get_arbitrary_time() == 999999999)

    def test_get(self) -> None:
        """
        Tests get()
        """
        success, value = self.data.get('UCOMVersion', self.dbu)
        assert(success)
        assert(value.get_value().u32 == self.signals[0].get_value(self.signals[0].generate_data(120)))

    def test_get_signal_count(self) -> None:
        """
        Tests get_signal_count()
        """
        assert(self.data.get_signal_count() == 3)
        

