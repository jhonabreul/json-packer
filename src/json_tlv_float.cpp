#include "json_tlv_float.hpp"

ByteArray JsonTLVFloat::serialize() const
{
    return nlohmann::json::to_cbor(this->value);
}

void JsonTLVFloat::deserialize(const ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVFloat::deserialize(ByteArrayIterator start, ByteArrayIterator end)
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
