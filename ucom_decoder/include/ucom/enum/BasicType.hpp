#pragma once

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

namespace OxTS
{
	namespace Enum
	{
		//!
		//! \brief BASIC_TYPE enum from NAVlib
		enum BASIC_TYPE
		{
			BASIC_TYPE_void,
			BASIC_TYPE_bool,
			BASIC_TYPE_char,
			BASIC_TYPE_float,
			BASIC_TYPE_double,
			BASIC_TYPE_uint8_t,
			BASIC_TYPE_uint16_t,
			BASIC_TYPE_uint32_t,
			BASIC_TYPE_uint64_t,
			BASIC_TYPE_int8_t,
			BASIC_TYPE_int16_t,
			BASIC_TYPE_int32_t,
			BASIC_TYPE_int64_t,
			BASIC_TYPE_enum_int64_t,
			BASIC_TYPE_str,
			BASIC_TYPE_UNKNOWN
		};
	}
}