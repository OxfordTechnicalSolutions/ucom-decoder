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

