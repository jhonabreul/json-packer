#include "json_tlv_string.hpp"

JsonTLVObject::ByteArray JsonTLVString::serialize() const
{
    return JsonTLVObject::ByteArray(this->value.begin(), this->value.end());
}

void JsonTLVString::deserialize(const JsonTLVObject::ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVString::deserialize(JsonTLVObject::ByteArrayIterator start,
                                JsonTLVObject::ByteArrayIterator end)
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
