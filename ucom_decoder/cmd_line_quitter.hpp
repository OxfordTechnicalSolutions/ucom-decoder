#pragma once

#include "quitter.hpp"
#include <thread>

//! @brief Provides an asynchronous way to check for user interaction on the 
//! command line requesting to quit a process
class CmdLineQuitter : public Quitter
{
private:
	std::thread _thread;
public:
	CmdLineQuitter();
	~CmdLineQuitter();
	void run();
};