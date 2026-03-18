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

        return super().setUp()

    def tearDown(self) -> None:
        return super().tearDown()
    
    def test_generate(self) -> None:
        # 'id', 'version', 'enabled', 'output_type', 'signals', and 'timing'
        signals = [UcomTestSignal('UCOMVersion','U32')]
        u = UcomTestMessage(1, 2, True, OutputTypes.OnTrigger, signals, TimeSources.TIME_SOURCE_GNSS)
        p, c = u.generate_packet(TimeFrames.SDN, 999999999, 120, GnssOffset())
        s = UcomData(p, len(p), self.dbu)
        assert(s.get_message_id() == 1)