#ifndef JSON_TLV_OBJECT_HPP
#define JSON_TLV_OBJECT_HPP

#include <vector>
#include <cstdint>
#include <tuple>

#include <nlohmann/json.hpp>

class JsonTLVObject
{
protected:

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

public:

    JsonTLVObject() {}

    JsonTLVObject(const ByteArray & bytes);

    ByteArray serialize() const;

    void deserialize(const ByteArray & bytes);

    virtual nlohmann::json toJson() const = 0;

protected:

    virtual JsonTLVObject::Tag getTag() const = 0;

    virtual ByteArray serializeValue() const = 0;

    virtual void deserializeValue(const ByteArray & bytes) = 0;

    virtual void deserializeValue(ByteArrayIterator start,
                                  ByteArrayIterator end) = 0;

    static ByteArray serializeTagAndLength(Tag tag, size_t length);

    static std::tuple<Tag, size_t, ByteArrayIterator>
    deserializeTagAndLength(const ByteArray & bytes);
};

template<class T>
class JsonTLVValueHolder: public JsonTLVObject
{
public:
    using ValueType = T;

protected:

    ValueType value;

public:

    JsonTLVValueHolder();

    JsonTLVValueHolder(const T & _value);

protected:

    ByteArray serializeValue() const override = 0;

    void deserializeValue(const ByteArray & bytes) override = 0;

    void deserializeValue(
        ByteArrayIterator start,
        ByteArrayIterator end) override = 0;

    virtual JsonTLVObject::Tag getTag() const override = 0;

public:

    nlohmann::json toJson() const override;

    operator const T&();
    operator T() const;
};

template<class T>
JsonTLVValueHolder<T>::JsonTLVValueHolder(): value(ValueType()) {}

template<class T>
JsonTLVValueHolder<T>::JsonTLVValueHolder(const T & _value): value(_value) {}

template<class T>
nlohmann::json JsonTLVValueHolder<T>::toJson() const
{
    return nlohmann::json(this->value);
}

template<class T>
JsonTLVValueHolder<T>::operator const T&()
{
    return this->value;
}

template<class T>
JsonTLVValueHolder<T>::operator T() const
{
    return this->value;
}

#endif // JSON_TLV_OBJECT_HPP
