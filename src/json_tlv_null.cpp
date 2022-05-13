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
    if (end - start > 0) {
        throw std::invalid_argument(
            "A null value is represented wihtout bytes and " +
            std::to_string(end - start) +
            " were received to deserialize");
    }
}

JsonTLVObject::Tag JsonTLVNull::getTag() const
{
    return Tag::Null;
}

nlohmann::json JsonTLVNull::toJson() const
{
    return this->value;
}
