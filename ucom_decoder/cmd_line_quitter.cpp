#include "cmd_line_quitter.hpp"
#include <iostream>
#include <sstream>
#include <chrono>

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
		std::cin >> input;
		if ((input.find("q") == 0) || (input.find("f") == 0))
			request_quit();
	}
}
