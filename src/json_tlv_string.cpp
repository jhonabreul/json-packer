#include "json_tlv_string.hpp"

ByteArray JsonTLVString::serialize() const
{
    return ByteArray(this->value.begin(), this->value.end());
}

void JsonTLVString::deserialize(const ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVString::deserialize(ByteArrayIterator start, ByteArrayIterator end)
{
    this->value = std::string(start, end);
}

JsonTLVObject::Tag JsonTLVString::getTag() const
{
    return Tag::String;
}

nlohmann::json JsonTLVString::toJson() const
{
    return this->value;
}
