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
#include "duration.hpp"

// @brief A simple class to faciliate monitoring elapsed time intervals
Duration::Duration(uint32_t seconds) :
	_duration_ms(seconds * 1000)
{
	_start_time = std::chrono::steady_clock::now();
}

// @brief Start the time interval
void Duration::start()
{
	_start_time = std::chrono::steady_clock::now();
}

// @brief Gets if the time interval has elapsed
// @returns true, if the time interval has elapsed, false otherwise
bool Duration::elapsed() const
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - _start_time).count() > _duration_ms;
}
