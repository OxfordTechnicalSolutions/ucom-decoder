#include "duration.hpp"

Duration::Duration(uint32_t seconds) :
	_duration_ms(seconds * 1000)
{
	_start_time = std::chrono::steady_clock::now();
}

void Duration::start()
{
	_start_time = std::chrono::steady_clock::now();
}

bool Duration::elapsed() const
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - _start_time).count() > _duration_ms;
}
