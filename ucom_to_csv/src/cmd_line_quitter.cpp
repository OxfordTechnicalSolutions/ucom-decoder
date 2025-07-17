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
#include "cmd_line_quitter.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#ifdef _WIN32
#include <conio.h>
#endif

//! @brief Provides an asynchronous way to check for user interaction on the 
//! command line requesting to quit a process
CmdLineQuitter::CmdLineQuitter()
{
	
}

// @brief Starts monitoring the command-line for input
void CmdLineQuitter::start()
{
	_thread = std::thread(&CmdLineQuitter::run, this);
}

CmdLineQuitter::~CmdLineQuitter()
{
	if (_thread.joinable())
		_thread.join();
}

void CmdLineQuitter::run()
{
	using namespace std::chrono_literals;
	std::string input;
	while (!is_quit_requested())
	{
#ifdef __linux__
		std::cin >> input;
		if ((input.find("q") == 0) || (input.find("f") == 0))
#elif _WIN32
		char ch = _getch();
		if ((ch == 'q') || (ch == 'f'))
#endif
			request_quit();

#ifdef _WIN32
		std::this_thread::sleep_for(1000ms);
#endif
	}
}
