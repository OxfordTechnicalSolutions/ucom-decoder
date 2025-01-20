#include "cmd_line_quitter.hpp"
#include <iostream>
#include <sstream>
#include <chrono>
#ifdef _WIN32
#include <conio.h>
#endif

CmdLineQuitter::CmdLineQuitter()
{
	
}


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
