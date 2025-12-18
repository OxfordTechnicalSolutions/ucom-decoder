#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

#include <map>

namespace UCOM
{

    /// Enumeration of currently supported data types for UCOM
    enum class DATA_TYPE
    {
        STR,    ///< String
        U8,     ///< Unsigned 8-bit integer
        S8,     ///< Signed 8-bit integer
        U16,    ///< Unsigned 16-bit integer
        S16,    ///< Signed 16-bit integer
        U32,    ///< Unsigned 32-bit integer
        S32,    ///< Signed 32-bit integer
        U64,    ///< Unsigned 64-bit integer
        S64,    ///< Signed 64-bit integer
        F32,    ///< 32-bit floating point
        F64,    ///< 64-bit floating point (double)
        EnS64,  ///< Enum unsigned 8-bit value and a signed 64-bit integer (72-bit total)
        INVALID ///< Invalid or unknown type
    };

#pragma pack(push, 1)
    struct EnS64 {
        uint8_t enum_member;
        int64_t value;
    };
#pragma pack(pop)
}


enum TimeSources {
    TIME_SOURCE_NONE,
    TIME_SOURCE_GNSS,
    TIME_SOURCE_PTP,
    TIME_SOURCE_EXT_GNSS,
    TIME_SOURCE_USER,
    TIME_SOURCE_GAD,
    TIME_SOURCE_UNKNOWN
};

std::string enumToString(TimeSources timeSource); 

class valueVariant
{
public:
    union Value
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
        std::string str;

        Value() :
            s32(0)
        {
        };
        ~Value() {}
    };
    
private:
    Value value;

    UCOM::DATA_TYPE value_type; // Data type
public:
    valueVariant(UCOM::DATA_TYPE v_data_type);

    valueVariant() : value_type(UCOM::DATA_TYPE::S32) {};

    valueVariant(const valueVariant& v);

    ~valueVariant() {
        if (value_type == UCOM::DATA_TYPE::STR)
            value.str.~basic_string();
    }

    bool operator== (const valueVariant& v) const;

    bool operator!= (const valueVariant& v) const { return !(this->operator==(v)); }
    
    /**
    * @brief Sets the variant to the value of another valueVariant instance.
    * @param v The valueVariant object to copy from.
    * @return void
    */
    void set_value(const valueVariant& v);

    /**
     * @brief Sets the variant to hold a string value.
     * @param s The std::string value to store.
     * @return void
     */
    void set_value(const std::string s);

    /**
     * @brief Sets the variant to hold an unsigned 8-bit integer.
     * @param v The uint8_t value to store.
     * @return void
     */
    void set_value(const uint8_t v);

    /**
     * @brief Sets the variant to hold a signed 8-bit integer.
     * @param v The int8_t value to store.
     * @return void
     */
    void set_value(const int8_t v);

    /**
     * @brief Sets the variant to hold an unsigned 16-bit integer.
     * @param v The uint16_t value to store.
     * @return void
     */
    void set_value(const uint16_t v);

    /**
     * @brief Sets the variant to hold a signed 16-bit integer.
     * @param v The int16_t value to store.
     * @return void
     */
    void set_value(const int16_t v);

    /**
     * @brief Sets the variant to hold an unsigned 32-bit integer.
     * @param v The uint32_t value to store.
     * @return void
     */
    void set_value(const uint32_t v);

    /**
     * @brief Sets the variant to hold a signed 32-bit integer.
     * @param v The int32_t value to store.
     * @return void
     */
    void set_value(const int32_t v);

    /**
     * @brief Sets the variant to hold an unsigned 64-bit integer.
     * @param v The uint64_t value to store.
     * @return void
     */
    void set_value(const uint64_t v);

    /**
     * @brief Sets the variant to hold an signed 64-bit integer.
     * @param v The uint64_t value to store.
     * @return void
     */
    void set_value(const int64_t v);

    /**
     * @brief Sets the variant to hold a single-precision floating-point value.
     * @param v The float value to store.
     * @return void
     */
    void set_value(const float v);

    /**
     * @brief Sets the variant to hold a double-precision floating-point value.
     * @param v The double value to store.
     * @return void
     */
    void set_value(const double v);

    /**
     * @brief Sets the variant to hold a signed 64-bit integer with an associated enum member.
     * @param v The int64_t value to store.
     * @param e The uint8_t enum member associated with the value.
     * @return void
     */
    void set_value(const int64_t v, const uint8_t e);


    /**
     * @brief Assigns an unsigned 8-bit integer value to the variant.
     * @param v The uint8_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const uint8_t v);

    /**
     * @brief Assigns a signed 8-bit integer value to the variant.
     * @param v The int8_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const int8_t v);

    /**
     * @brief Assigns an unsigned 16-bit integer value to the variant.
     * @param v The uint16_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const uint16_t v);

    /**
     * @brief Assigns a signed 16-bit integer value to the variant.
     * @param v The int16_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const int16_t v);

    /**
     * @brief Assigns an unsigned 32-bit integer value to the variant.
     * @param v The uint32_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const uint32_t v);

    /**
     * @brief Assigns a signed 32-bit integer value to the variant.
     * @param v The int32_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const int32_t v);

    /**
     * @brief Assigns an unsigned 64-bit integer value to the variant.
     * @param v The uint64_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const uint64_t v);

    /**
     * @brief Assigns a signed 64-bit integer value to the variant.
     * @param v The int64_t value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const int64_t v);

    /**
     * @brief Assigns a single-precision floating-point value to the variant.
     * @param v The float value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const float v);

    /**
     * @brief Assigns a double-precision floating-point value to the variant.
     * @param v The double value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const double v);

    /**
     * @brief Assigns a string value to the variant.
     * @param v The std::string value to assign.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const std::string v);

    /**
     * @brief Assigns the value from another valueVariant instance.
     * @param v The valueVariant instance to copy from.
     * @return Reference to the modified valueVariant instance.
     */
    valueVariant& operator=(const valueVariant & v);

    const Value& get_value();

    const UCOM::DATA_TYPE& get_type();

    std::string to_string() const;
};

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
        return std::static_pointer_cast<T>(_value_ptr);
    }

    std::string type()
    {
        return _type;
    }



};