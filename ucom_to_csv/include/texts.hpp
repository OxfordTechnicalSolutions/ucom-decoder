
/*
   Copyright © 2025 Oxford Technical Solutions (OxTS)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

const char* help_text[] = {
	"Usage: ucom_to_csv [options] -u <.dbu filename>",
	"Options:",
	"  -h                     Help - displays this message",
	"  -f <input file>        Extract data from a file instead of UDP stream",
	"  -m <id1 id2 id3 ...>   Message IDs to process in the format {id}v{version}, e.g. 0 1 3 1v1 3v1 2v1",
	"  -c <packets>           Number of packets to process (UDP stream only)",
	"  -t <duration>          Maximum capture duration in seconds (UDP stream only)",
	"  -i <address>           Source IP address (UDP stream only)", 
	"  -o <output file>       Output file prefix (default is output_)",
	"  -a                     Disable user-abort"
};

const char* banner_text[] = {
	"**************************************************",
	"* UCOM to CSV Tool 0.3.251030                    *",
	"* Oxford Technical Solutions Ltd 2025            *",
	"**************************************************"
};
