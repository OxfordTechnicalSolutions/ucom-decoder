import unittest
import json
from oxts.ucompy import UcomDbu, UcomMessage, UCOM_DATA_TYPE, UCOM_TRIGGERS
import data as test_data

class UcomDbuTest(unittest.TestCase):
    def setUp(self) -> None:
        self.dbu = UcomDbu("mobile.dbu")
        return super().setUp()
    
    def tearDown(self) -> None:
    
        return super().tearDown()
    
    def test_get_valid(self):
        """
        Tests get_valid()
        """
        assert(self.dbu.get_valid())

    def test_get_filename(self):
        """
        Tests get_filename()
        """
        assert(self.dbu.get_filename() == "mobile.dbu")

    def test_message_id_exists(self):
        """
        Tests message_id_exists()
        """
        assert(self.dbu.message_id_exists(64512))
        assert(not self.dbu.message_id_exists(64513))

    def test_message_uid_exists(self):
        """
        Tests message_uid_exists()
        """
        assert(self.dbu.message_uid_exists(130048))
        assert(not self.dbu.message_uid_exists(130049))

    def test_message_get_messages(self):
        """
        Tests get_messages()
        """
        messages = self.dbu.get_messages()
        id : int
        message : UcomMessage
        for uid, message in messages.items():
            assert(self.dbu.message_uid_exists(uid))
            assert(message.get_uid() == uid)

    def test_get_message_ids(self):
        """
        Tests get_message_ids()
        """

        for id in self.dbu.get_message_ids():
            assert(self.dbu.message_id_exists(id))

    def test_get_message_uids(self):
        """
        Tests get_message_uids()
        """

        for uid in self.dbu.get_message_uids():
            assert(self.dbu.message_uid_exists(uid))

    def test_get_message(self):
        """
        Tests get_message()
        """

        assert(type(self.dbu.get_message(64512)) is UcomMessage)
        assert(self.dbu.get_message(64512).get_id() == 64512)

    def test_get_signals(self):
        """
        Tests get_signals()
        """
        signals = self.dbu.get_signals(64512)
        assert(len(signals) == 3)
        assert(signals[0].get_signal_id() == "UCOMVersion")

    def test_get_ucom_data_type(self):
        """
        Tests get_ucom_data_type()
        """

        assert(UcomDbu.get_ucom_data_type("U8") == UCOM_DATA_TYPE.U8)
    
    def test_get_trigger_name(self):
        """
        Tests get_trigger_name()
        """
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.NO_TRIGGER) == "No Trigger")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.IN_1_DOWN) == "In 1 Down")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.IN_1_UP) == "In 1 Up")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.OUT_1) == "Out 1")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.IN_2_DOWN) == "In 2 Down")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.IN_2_UP) == "In 2 Up")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.UNKNOWN_1) == "Unknown 1")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.UNKNOWN_2) == "Unknown 2")
        assert(self.dbu.get_trigger_name(UCOM_TRIGGERS.OUT_2) == "Out 2")
        

"""

std::string get_header_timing_name(const std::string& timing) const;"""
