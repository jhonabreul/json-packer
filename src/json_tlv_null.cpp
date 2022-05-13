#include "json_tlv_null.hpp"

ByteArray JsonTLVNull::serialize() const
{
    return {};
}

void JsonTLVNull::deserialize(const ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVNull::deserialize(ByteArrayIterator start, ByteArrayIterator end)
{
    // TODO: Maybe this should be an exeption
    assert(end - start == 0);
}

JsonTLVObject::Tag JsonTLVNull::getTag() const
{
    return Tag::Null;
}

nlohmann::json JsonTLVNull::toJson() const
{
    return this->value;
}
