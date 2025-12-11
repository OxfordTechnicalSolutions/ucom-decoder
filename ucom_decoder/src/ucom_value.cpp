#pragma once
#include "ucom/ucom_value.hpp"
#include <memory>
#include <string>

UcomValue::UcomValue(std::shared_ptr<void> ptr_to_value, std::string type) :
	_value_ptr{ ptr_to_value },
	_type{ type }
{

}

/**
 * @brief Converts a TimeSources enum value to its corresponding string representation.
 *
 * @param timeSource The TimeSources enum value to convert.
 * @return A std::string representing the name of the enum value.
 */
std::string enumToString(TimeSources timeSource) {
    static const std::map<TimeSources, std::string> sourceMap = {
        { TimeSources::TIME_SOURCE_NONE, "SDN" },
        { TimeSources::TIME_SOURCE_GNSS, "GNSS" },
        { TimeSources::TIME_SOURCE_PTP, "PTP" },
        { TimeSources::TIME_SOURCE_EXT_GNSS, "ExtGNSS"},
        { TimeSources::TIME_SOURCE_USER, "User"},
        { TimeSources::TIME_SOURCE_GAD, "GAD"},
        { TimeSources::TIME_SOURCE_UNKNOWN, "Unknown"}
    };
    auto it = sourceMap.find(timeSource);
    return (it != sourceMap.end()) ? it->second : "Unknown";
}

/**
 * @brief Constructs a valueVariant object with the specified data type.
 *
 * Initializes the valueVariant instance by setting its internal value type
 * to the provided UCOM::DATA_TYPE.
 *
 * @param v_data_type The data type used to initialize the valueVariant.
 */
valueVariant::valueVariant(UCOM::DATA_TYPE v_data_type)
    : value_type(v_data_type)
{
    switch (value_type)
    {
    case UCOM::DATA_TYPE::STR:
        new(&value.str) std::string;  // Constructor for string
        break;
    case UCOM::DATA_TYPE::U8:
        set_value(static_cast<uint8_t>(0));
        break;
    case UCOM::DATA_TYPE::S8:
        set_value(static_cast<int8_t>(0));
        break;
    case UCOM::DATA_TYPE::U16:
        set_value(static_cast<uint16_t>(0));
        break;
    case UCOM::DATA_TYPE::S16:
        set_value(static_cast<int16_t>(0));
        break;
    case UCOM::DATA_TYPE::U32:
        set_value(static_cast<uint32_t>(0));
        break;
    case UCOM::DATA_TYPE::S32:
        set_value(static_cast<int32_t>(0));
        break;
    case UCOM::DATA_TYPE::U64:
        set_value(static_cast<uint64_t>(0));
        break;
    case UCOM::DATA_TYPE::S64:
        set_value(static_cast<int64_t>(0));
        break;
    case UCOM::DATA_TYPE::F32:
        set_value(static_cast<float>(0));
        break;
    case UCOM::DATA_TYPE::F64:
        set_value(static_cast<double>(0));
        break;
    case UCOM::DATA_TYPE::EnS64:
        set_value(0, 0);
        break;
    default:
        set_value(nan(""));
    }
}

/**
 * @brief Copy constructor for the valueVariant class.
 * @param v The valueVariant object to copy from.
 */
valueVariant::valueVariant(const valueVariant& v)
{
    set_value(v);
}

bool valueVariant::operator== (const valueVariant& v) const
{
    if (v.value_type != value_type)
        return false;

    switch (value_type)
    {
    case UCOM::DATA_TYPE::STR:
        return value.str == v.value.str;
    case UCOM::DATA_TYPE::U8:
        return value.u8 == v.value.u8;
    case UCOM::DATA_TYPE::S8:
        return value.s8 == v.value.s8;
    case UCOM::DATA_TYPE::U16:
        return value.u16 == v.value.u16;
    case UCOM::DATA_TYPE::S16:
        return value.s16 == v.value.s16;
    case UCOM::DATA_TYPE::U32:
        return value.u32 == v.value.u32;
    case UCOM::DATA_TYPE::S32:
        return value.s32 == v.value.s32;
    case UCOM::DATA_TYPE::U64:
        return value.u64 == v.value.u64;
    case UCOM::DATA_TYPE::S64:
        return value.s64 == v.value.s64;
    case UCOM::DATA_TYPE::F32:
        return value.f32 == v.value.f32;
    case UCOM::DATA_TYPE::F64:
        return value.f64 == v.value.f64;
    case UCOM::DATA_TYPE::EnS64:
        return (value.ens64.enum_member == v.value.ens64.enum_member) && (value.ens64.value == v.value.ens64.value);
    default:
        return false;
    }
}

/**
 * @brief Sets the value of this valueVariant instance from another valueVariant.
 * If the type is unrecognized, the value is set to NaN.
 *
 * @param v The valueVariant instance whose value is to be copied.
 */
void valueVariant::set_value(const valueVariant& v)
{
    switch (v.value_type)
    {
    case UCOM::DATA_TYPE::STR:
        set_value(v.value.str);
        break;
    case UCOM::DATA_TYPE::U8:
        set_value(v.value.u8);
        break;
    case UCOM::DATA_TYPE::S8:
        set_value(v.value.s8);
        break;
    case UCOM::DATA_TYPE::U16:
        set_value(v.value.u16);
        break;
    case UCOM::DATA_TYPE::S16:
        set_value(v.value.s16);
        break;
    case UCOM::DATA_TYPE::U32:
        set_value(v.value.u32);
        break;
    case UCOM::DATA_TYPE::S32:
        set_value(v.value.s32);
        break;
    case UCOM::DATA_TYPE::U64:
        set_value(v.value.u64);
        break;
    case UCOM::DATA_TYPE::S64:
        set_value(v.value.s64);
        break;
    case UCOM::DATA_TYPE::F32:
        set_value(v.value.f32);
        break;
    case UCOM::DATA_TYPE::F64:
        set_value(v.value.f64);
        break;
    case UCOM::DATA_TYPE::EnS64:
        set_value(v.value.ens64.value, v.value.ens64.enum_member);
        break;
    default:
        set_value(nan(""));
    }
}

/**
 * @brief Sets the value of the valueVariant instance using a specific data type.
 * 
 * @param v The value to assign, with the method selected based on the type of `v`.
 */
void valueVariant::set_value(const std::string s)
{
    value_type = UCOM::DATA_TYPE::STR;
    new(&value.str) std::string(s);
}

void valueVariant::set_value(const uint8_t v)
{
    value_type = UCOM::DATA_TYPE::U8;
    value.u8 = v;
}

void valueVariant::set_value(const int8_t v)
{
    value_type = UCOM::DATA_TYPE::S8;
    value.s8 = v;
}

void valueVariant::set_value(const uint16_t v)
{
    value_type = UCOM::DATA_TYPE::U16;
    value.u16 = v;
}

void valueVariant::set_value(const int16_t v)
{
    value_type = UCOM::DATA_TYPE::S16;
    value.s16 = v;
}

void valueVariant::set_value(const uint32_t v)
{
    value_type = UCOM::DATA_TYPE::U32;
    value.u32 = v;
}

void valueVariant::set_value(const int32_t v)
{
    value_type = UCOM::DATA_TYPE::S32;
    value.s32 = v;
}

void valueVariant::set_value(const uint64_t v)
{
    value_type = UCOM::DATA_TYPE::U64;
    value.u64 = v;
}

void valueVariant::set_value(const int64_t v)
{
    value_type = UCOM::DATA_TYPE::S64;
    value.s64 = v;
}

void valueVariant::set_value(const float v)
{
    value_type = UCOM::DATA_TYPE::F32;
    value.f32 = v;
}

void valueVariant::set_value(const double v)
{
    value_type = UCOM::DATA_TYPE::F64;
    value.f64 = v;
}

void valueVariant::set_value(const int64_t v, const uint8_t e)
{
    value_type = UCOM::DATA_TYPE::EnS64;
    value.ens64.value = v;
    value.ens64.enum_member = e;
}

/**
 * @brief Provides access to the internal value storage of the valueVariant.
 *
 * @return Reference to the internal Value structure.
 */
const valueVariant::Value& valueVariant::get_value()
{
    return value;
}

/**
 * @brief Retrieves the current data type of the stored value.
 *
 * @return Reference to the UCOM::DATA_TYPE representing the current value type.
 */
const UCOM::DATA_TYPE& valueVariant::get_type()
{
    return value_type;
}


/**
 * @brief Overloaded assignment operators for assigning primitive values to a valueVariant instance.
 *
 * @param v The value to assign.
 * @return Reference to the updated valueVariant instance.
 */
valueVariant& valueVariant::operator=(const uint8_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const int8_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const uint16_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const int16_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const uint32_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const int32_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const uint64_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const int64_t v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const float v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const double v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const std::string v)
{
    set_value(v);
    return *this;
}

valueVariant& valueVariant::operator=(const valueVariant& v)
{
    set_value(v);
    return *this;
}

/**
 * @brief Converts the stored value to its string representation.
 *
 * If the type is unrecognized, the value is set to 'nan'.
 *
 * @return A std::string containing the formatted value.
 */
std::string valueVariant::to_string() const
{
    std::stringstream ss;
    switch (value_type)
    {
    case UCOM::DATA_TYPE::U8:
        ss << static_cast<int>(value.u8);
        break;
    case UCOM::DATA_TYPE::S8:
        ss << static_cast<int>(value.s8);
        break;
    case UCOM::DATA_TYPE::U16:
        ss << value.u16;
        break;
    case UCOM::DATA_TYPE::S16:
        ss << value.s16;
        break;
    case UCOM::DATA_TYPE::U32:
        ss << value.u32;
        break;
    case UCOM::DATA_TYPE::S32:
        ss << value.s32;
        break;
    case UCOM::DATA_TYPE::U64:
        ss << value.u64;
        break;
    case UCOM::DATA_TYPE::S64:
        ss << value.s64;
        break;
    case UCOM::DATA_TYPE::F32:
        ss.precision(9);
        ss << std::scientific << value.f32;
        break;
    case UCOM::DATA_TYPE::F64:
        ss.precision(15);
        ss << std::scientific << value.f64;
        break;
    case UCOM::DATA_TYPE::STR:
        ss << value.str;
        break;
    case UCOM::DATA_TYPE::EnS64:
        ss << enumToString(static_cast<TimeSources>(value.ens64.enum_member)) << "," << value.ens64.value;
        break;
    default:
        ss << nan("");

    }
    return ss.str();
}