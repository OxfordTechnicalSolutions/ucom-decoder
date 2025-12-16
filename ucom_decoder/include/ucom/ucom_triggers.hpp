#pragma once
#include <map>
namespace UCOM {
	enum class TRIGGER_TYPES : uint8_t
	{
		NO_TRIGGER,
		IN_1_DOWN,
		IN_1_UP,
		OUT_1,
		IN_2_DOWN,
		IN_2_UP,
		UNKNOWN_1,
		UNKNOWN_2,
		OUT_2
	};
}

