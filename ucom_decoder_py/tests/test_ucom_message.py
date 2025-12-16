import unittest
import json
from oxts.ucompy import UcomMessage
import data as test_data

class UcomMessageTest(unittest.TestCase):
    def setUp(self) -> None:
        self.message_str = json.dumps(test_data.message_dict)
        self.message = UcomMessage(self.message_str)

        return super().setUp()
    
    def tearDown(self) -> None:
        return super().tearDown()
    
    def test_is_valid(self):
        """
        Tests is_valid()
        """
        assert(self.message.is_valid())

    def test_get_id(self):
        """
        Tests get_id()
        """
        assert(self.message.get_id() == 64512)

    def test_get_uid(self):
        """
        Tests get_uid()
        """
        assert(self.message.get_uid() == 64512)

    def test_get_header(self):
        """
        Tests get_header()
        """
        header = self.message.get_header()
        assert(header == "Time (SDN),UCOMVersion,DevID,Wd")

    def test_get_signal_count(self):
        """
        Tests get_signal_count()
        """
        assert(self.message.get_signal_count() == 3)

    def test_get_signals(self):
        """
        Tests get_signals()
        """
        signals = self.message.get_signals()
        assert(len(signals) == 3)

    def test_get_enabled(self):
        """
        Tests get_enabled()
        """
    
        assert(self.message.get_enabled() == False)

    def test_get_signal_index(self):
        """
        Tests get_signal_index()
        """
        assert(self.message.get_signal_index("Wd") == 2)
        
    
    def test_get_id_from_uid(self):
        """
        Tests extracting the message ID from the UID
        """
        assert(UcomMessage.get_id_from_uid(0) == 0)
        assert(UcomMessage.get_id_from_uid(256) == 256)
        assert(UcomMessage.get_id_from_uid(65535) == 65535)
        assert(UcomMessage.get_id_from_uid(65536) == 0)
        assert(UcomMessage.get_id_from_uid(65537) == 1)
        assert(UcomMessage.get_id_from_uid(131072) == 0)
        assert(UcomMessage.get_id_from_uid(131073) == 1)
        assert(UcomMessage.get_id_from_uid(196607) == 65535)
        assert(UcomMessage.get_id_from_uid(262144) == 0)
        assert(UcomMessage.get_id_from_uid(262145) == 1)
        assert(UcomMessage.get_id_from_uid(327679) == 65535)

        
    def test_get_version_from_uid(self):
        """
        Tests extracting the message version from the UID
        """
        assert(UcomMessage.get_version_from_uid(0) == 0)
        assert(UcomMessage.get_version_from_uid(256) == 0)
        assert(UcomMessage.get_version_from_uid(65535) == 0)
        assert(UcomMessage.get_version_from_uid(65536) == 1)
        assert(UcomMessage.get_version_from_uid(131072) == 2)
        assert(UcomMessage.get_version_from_uid(131073) == 2)
        assert(UcomMessage.get_version_from_uid(196607) == 2)
        assert(UcomMessage.get_version_from_uid(262144) == 4)
        assert(UcomMessage.get_version_from_uid(262145) == 4)
        assert(UcomMessage.get_version_from_uid(327679) == 4)

    def test_create_uid(self):
        """
        Tests creating message UIDs
        """
        assert(UcomMessage.create_uid(0, 0) == 0)
        assert(UcomMessage.create_uid(1, 0) == 1)
        assert(UcomMessage.create_uid(65535, 0) == 65535)
        assert(UcomMessage.create_uid(0, 1) == 65536)
        assert(UcomMessage.create_uid(65535, 1) == 131071)
        assert(UcomMessage.create_uid(0, 2) == 131072)
        assert(UcomMessage.create_uid(65535, 2) == 196607)
        assert(UcomMessage.create_uid(0, 4) == 262144)
        assert(UcomMessage.create_uid(65535, 4) == 327679)
        assert(UcomMessage.create_uid(0, 256) == 16777216)
        assert(UcomMessage.create_uid(65535, 256) == 16842751)
        
    def test_uid_to_string(self):
        """
        Tests UID to string
        """
        uid_str = UcomMessage.uid_to_string(0)
        assert(uid_str == "0")
        
    def test_uid_from_string(self):
        """
        Tests UID from string
        """
        success, uid = UcomMessage.uid_from_string("0")
        assert(uid == 0)
        success, uid = UcomMessage.uid_from_string("1")
        assert(uid == 1)
        success, uid = UcomMessage.uid_from_string("65535")
        assert(uid == 65535)
        success, uid = UcomMessage.uid_from_string("65535v1")
        assert(uid == 131071)
        success, uid = UcomMessage.uid_from_string("65535v3")
        assert(uid == 262143)

    def test_get_signal_index(self):
        """
        Tests get_signal_index()
        """
        assert(self.message.get_signal_index("UCOMVersion") == 0)
