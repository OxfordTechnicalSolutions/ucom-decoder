#pragma once
#include <cstdint>
#include <chrono>

class Duration
{
private:
	uint32_t _duration_ms;
	std::chrono::steady_clock::time_point _start_time;
public:
	Duration(uint32_t seconds);
	void start();
	bool elapsed() const;
};