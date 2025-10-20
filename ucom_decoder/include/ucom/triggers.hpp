#pragma once

class Triggers
{
public:
	enum class Types : uint8_t
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
private:
	static const std::map<Types, std::string> TRIGGER_TYPE_NAMES;

};

