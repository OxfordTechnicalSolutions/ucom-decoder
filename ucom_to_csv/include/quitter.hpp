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