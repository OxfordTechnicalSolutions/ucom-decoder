#pragma once
#include "args.hpp"

class Application
{
private:

protected:
	Args _args;

public:
	Application(int argc, char *argv[]);
	virtual int run();
};