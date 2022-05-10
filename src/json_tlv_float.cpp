#include "json_tlv_float.hpp"

JsonTLVObject::ByteArray JsonTLVFloat::serialize() const
{
    return nlohmann::json::to_cbor(this->value);
}

void JsonTLVFloat::deserialize(const JsonTLVObject::ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVFloat::deserialize(JsonTLVObject::ByteArrayIterator start,
                               JsonTLVObject::ByteArrayIterator end)
{
    this->value = nlohmann::json::from_cbor(start, end).get<ValueType>();
}

JsonTLVObject::Tag JsonTLVFloat::getTag() const
{
    return Tag::Float;
}

nlohmann::json JsonTLVFloat::toJson() const
{
    return this->value;
}
