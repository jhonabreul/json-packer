#ifndef JSON_TLV_OBJECT_HPP
#define JSON_TLV_OBJECT_HPP

#include <vector>
#include <cstdint>
#include <tuple>
#include <functional>
#include <utility>
#include <memory>

#include <nlohmann/json.hpp>

class JsonTLVObject
{
public:

    using Byte = uint8_t;
    using ByteArray = std::vector<uint8_t>;
    using ByteArrayIterator = std::vector<uint8_t>::const_iterator;

    enum class Tag: uint8_t
    {
        Record,
        Integer,
        Float,
        Boolean,
        String,
        Null
    };

    JsonTLVObject() {}

    virtual ByteArray serialize() const = 0;

    virtual void deserialize(const ByteArray & bytes) = 0;

    virtual void deserialize(ByteArrayIterator start,
                             ByteArrayIterator end) = 0;

    virtual JsonTLVObject::Tag getTag() const = 0;

    virtual nlohmann::json toJson() const = 0;

    virtual bool equalTo(const JsonTLVObject & other) const;

    static ByteArray serializeTagAndLength(Tag tag, size_t length);

    static std::tuple<Tag, size_t, ByteArrayIterator>
    deserializeTagAndLength(const ByteArray & bytes);

    static std::tuple<Tag, size_t, ByteArrayIterator>
    deserializeTagAndLength(ByteArrayIterator start, ByteArrayIterator end);
};

template<class T, class EqualTo = std::equal_to<T>>
class JsonTLVValueHolder: public JsonTLVObject
{
public:
    using ValueType = T;

protected:

    ValueType value;

public:

    using JsonTLVObject::JsonTLVObject;

    JsonTLVValueHolder();

    JsonTLVValueHolder(const T & _value);

    JsonTLVValueHolder(T && _value);

    // TODO: Copy and move constructors

    virtual JsonTLVObject::Tag getTag() const override = 0;

    ByteArray serialize() const override = 0;

    void deserialize(const ByteArray & bytes) override = 0;

    void deserialize(ByteArrayIterator start,
                     ByteArrayIterator end) override = 0;

    virtual nlohmann::json toJson() const override = 0;

    operator const T&();
    operator T() const;

    bool equalTo(const JsonTLVObject & other) const override;
};

template<class T, class EqualTo>
JsonTLVValueHolder<T, EqualTo>::JsonTLVValueHolder(): value(ValueType()) {}

template<class T, class EqualTo>
JsonTLVValueHolder<T, EqualTo>::JsonTLVValueHolder(const T & _value)
    : value(_value)
{}

template<class T, class EqualTo>
JsonTLVValueHolder<T, EqualTo>::JsonTLVValueHolder(T && _value)
    : value(std::move(_value))
{}

template<class T, class EqualTo>
JsonTLVValueHolder<T, EqualTo>::operator const T&()
{
    return this->value;
}

template<class T, class EqualTo>
JsonTLVValueHolder<T, EqualTo>::operator T() const
{
    return this->value;
}

template<class T, class EqualTo>
bool JsonTLVValueHolder<T, EqualTo>::equalTo(const JsonTLVObject & other) const
{
    using Holder = JsonTLVValueHolder<T, EqualTo>;
    if (const Holder* other_ptr = dynamic_cast<const Holder*>(&other)) {
        return EqualTo()(this->value, other_ptr->value);
    }

    return false;
}

#endif // JSON_TLV_OBJECT_HPP
