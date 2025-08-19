#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

namespace UCOM
{

    /* Data types Enum */
    enum class DATA_TYPE
    {
        STR,
        U8,
        S8,
        U16,
        S16,
        U24,
        U32,
        S32,
        S64,
        U64,
        F32,
        F64,
        EnS64,
        INVALID
    };

#pragma pack(push, 1)
    struct EnS64 {
        uint8_t enum_member;
        int64_t value;
    };
#pragma pack(pop)
}

#include <map>

enum TimeSources {
    TIME_SOURCE_NONE,
    TIME_SOURCE_GNSS,
    TIME_SOURCE_PTP,
    TIME_SOURCE_EXT_GNSS,
    TIME_SOURCE_USER,
    TIME_SOURCE_GAD,
    TIME_SOURCE_UNKNOWN, 
    TIME_SOURCE_TOM = 9
};

std::string enumToString(TimeSources timeSource); 

struct valueVariant
{
    union
    {
        uint8_t  u8;
        int8_t   s8;
        uint16_t u16;
        int16_t  s16;
        // uint32_t u24;
        uint32_t u32;
        int32_t  s32;
        int64_t  s64;
        uint64_t u64;
        float    f32;
        double   f64;
        UCOM::EnS64 ens64;
    } value;

    UCOM::DATA_TYPE value_type; // Data type

    valueVariant(UCOM::DATA_TYPE v_data_type)
        : value_type(v_data_type)
    {
    }

    valueVariant() : value_type(UCOM::DATA_TYPE::S32) {};

    std::string to_string() const
    {
        std::stringstream ss;
        switch (value_type)
        {
        case UCOM::DATA_TYPE::U8:
            ss << (int)value.u8;
            break;
        case UCOM::DATA_TYPE::S8:
            ss << (int)value.s8;
            break;

        case UCOM::DATA_TYPE::EnS64:
            // ss << (int)value.ens64.enum_member << value.ens64.value;
            ss << enumToString(static_cast<TimeSources>(value.ens64.enum_member)) << "," << value.ens64.value;
            break;
        default:
            ss << std::fixed << value.f64;
                
        }
        return ss.str();
    }
};




/*
double convert_variant_value_to_double(const valueVariant& variant)
{
    double value = nan("");

    switch (variant.value_type)
    {
    case UCOM::DATA_TYPE::U8:
    {
        value = static_cast<double>(variant.value.u8);
    }
    break;
    case UCOM::DATA_TYPE::S8:
    {
        value = static_cast<double>(variant.value.s8);
    }
    break;
    case UCOM::DATA_TYPE::U16:
    {
        value = static_cast<double>(variant.value.u16);
    }
    break;
    case UCOM::DATA_TYPE::S16:
    {
        value = static_cast<double>(variant.value.s16);
    }
    break;
    case UCOM::DATA_TYPE::U32:
    {
        value = static_cast<double>(variant.value.u32);
    }
    break;
    case UCOM::DATA_TYPE::S32:
    {
        value = static_cast<double>(variant.value.s32);
    }
    break;
    case UCOM::DATA_TYPE::S64:
    {
        value = static_cast<double>(variant.value.s64);
    }
    break;
    case UCOM::DATA_TYPE::U64:
    {
        value = static_cast<double>(variant.value.u64);
    }
    break;
    case UCOM::DATA_TYPE::F32:
    {
        value = static_cast<double>(variant.value.f32);
    }
    break;
    case UCOM::DATA_TYPE::F64:
    {
        value = static_cast<double>(variant.value.f64);
    }
    break;
    case UCOM::DATA_TYPE::INVALID:
    default:
        return value;
    }
    return value;
}
*/

class UcomValue {
private:
    std::string _type = "S32"; // default is 32-bit signed int
    std::shared_ptr<void> _value_ptr;

public:
    UcomValue(std::shared_ptr<void> ptr_to_value, std::string type);

    template<typename T>
    UcomValue(T value, std::string type) :
        _type{ type }
    {
        _value_ptr = std::make_shared<T>(value);
    }

    template<typename T>
    std::shared_ptr<T> get()
    {
        return static_pointer_cast<T>(_value_ptr);
    }

    std::string type()
    {
        return _type;
    }

    inline static std::vector<std::string> TYPES = {
        "B1",
        "S8",
        "U8",
        "S16",
        "U16",
        "U24",
        "S32",
        "U32",
        "S64",
        "U64",
        "F32",
        "F64",
        "EnS64",
        "STR"
    };
};