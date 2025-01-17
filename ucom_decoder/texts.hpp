#pragma once

const char* help_text[] = {
	"Usage: ucom_decoder [options] -u <.dbu filename>",
	"Options:",
	"  -h                     Help - displays this message",
	"  -f <input file>        Extract data from a file instead of UDP stream",
	"  -m <id1 id2 id3 ...>   Message IDs to process, e.g. 0 1 3",
	"  -c <packets>           Number of packets to process (UDP stream only)",
	"  -t <duration>          Maximum capture duration in seconds (UDP stream only)",
	"  -i <address>           Source IP address (UDP stream only)", 
	"  -o <output file>       Output file prefix (default is output_)"
};

const char* banner_text[] = {
	"**************************************************",
	"* UCOM Decoder                                   *",
	"* Oxford Technical Solutions Ltd 2025            *",
	"**************************************************"
};