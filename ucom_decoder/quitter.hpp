#pragma once
#include <mutex>

//! @brief A thread-safe class to manage requests to quit (e.g. a process / application)
class Quitter
{
protected:
	bool _quit_requested = false;
	std::mutex _quit_mutex;
public:
	//! @brief Gets if quit is requested
	//! @return true if quit requested, false otherwise
	bool is_quit_requested();

	//! @brief Request quit
	void request_quit();

	//! @brief Reset (cancels quit request)
	void reset();
};