#pragma once
#include "ucom/ucom_value.hpp"
#include <memory>
#include <string>

UcomValue::UcomValue(std::shared_ptr<void> ptr_to_value, std::string type) :
	_value_ptr{ ptr_to_value },
	_type{ type }
{

}

std::string enumToString(TimeSources timeSource) {
    static const std::map<TimeSources, std::string> sourceMap = {
        { TimeSources::TIME_SOURCE_NONE, "SDN" },
        { TimeSources::TIME_SOURCE_GNSS, "GNSS" },
        { TimeSources::TIME_SOURCE_PTP, "PTP" },
        { TimeSources::TIME_SOURCE_EXT_GNSS, "ExtGNSS"},
        { TimeSources::TIME_SOURCE_USER, "User"},
        { TimeSources::TIME_SOURCE_GAD, "GAD"},
        { TimeSources::TIME_SOURCE_UNKNOWN, "Unknown"},
        { TimeSources::TIME_SOURCE_TOM, "Tom Time"}
    };
    auto it = sourceMap.find(timeSource);
    return (it != sourceMap.end()) ? it->second : "Unknown";
}