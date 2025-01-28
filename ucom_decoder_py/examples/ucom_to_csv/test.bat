@ECHO OFF

ECHO OxTS UCOM Python SDK Test

SET DBU=ucom_decoder\example_data\dbu\oxts.dbu
SET INPUT_FILE=ucom_decoder\example_data\input.ucom
SET OUTPUT_FILE_PREFIX=oxts_out_

py ucom_decoder/ucom_decoder_py/examples/ucom_to_csv/ucom_to_csv.py^
	-u %DBU%^
	-f %INPUT_FILE%^
	-o %OUTPUT_FILE_PREFIX%
