#include "quitter.hpp"

void Quitter::request_quit()
{
	std::lock_guard<std::mutex> lock(_quit_mutex);
	_quit_requested = true;
}

void Quitter::reset()
{
	std::lock_guard<std::mutex> lock(_quit_mutex);
	_quit_requested = false;
}

bool Quitter::is_quit_requested()
{
	std::lock_guard<std::mutex> lock(_quit_mutex);
	return _quit_requested;
}

