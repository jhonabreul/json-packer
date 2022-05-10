#include "json_tlv_null.hpp"

JsonTLVObject::ByteArray JsonTLVNull::serialize() const
{
    return {};
}

void JsonTLVNull::deserialize(const JsonTLVObject::ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVNull::deserialize(JsonTLVObject::ByteArrayIterator start,
                              JsonTLVObject::ByteArrayIterator end)
{
    // TODO: Maybe this should be an exeption
    assert(end- start == 0);
}

JsonTLVObject::Tag JsonTLVNull::getTag() const
{
    return Tag::Null;
}

nlohmann::json JsonTLVNull::toJson() const
{
    return this->value;
}
