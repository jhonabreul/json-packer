#include "json_tlv_object.hpp"

#include "serialization.hpp"

JsonTLVObject::JsonTLVObject(const ByteArray & bytes)
{
    this->deserialize(bytes);
}

JsonTLVObject::ByteArray JsonTLVObject::serialize() const
{
    auto value_bytes = this->serializeValue();
    auto bytes = JsonTLVObject::serializeTagAndLength(this->getTag(),
                                                      value_bytes.size());

    bytes.insert(bytes.end(), value_bytes.begin(), value_bytes.end());

    return bytes;
}

void JsonTLVObject::deserialize(const JsonTLVObject::ByteArray & bytes)
{
    JsonTLVObject::Tag tag;
    size_t length;
    JsonTLVObject::ByteArrayIterator value_start_it;

    std::tie(tag, length, value_start_it) =
        JsonTLVObject::deserializeTagAndLength(bytes);
    assert(length <= bytes.end() - value_start_it);
    this->deserializeValue(value_start_it, value_start_it + length);
}

JsonTLVObject::ByteArray JsonTLVObject::serializeTagAndLength(Tag tag,
                                                          size_t length)
{
    auto serialized_length = serializeNumericValue(length);
    assert(serialized_length.size() <= 0x0F);
    auto serialized_tag = serializeNumericValue(static_cast<uint8_t>(tag));
    assert(serialized_tag.size() == 1);

    serialized_tag[0] <<= 5;

    if (serialized_length.size() == 1 && serialized_length[0] <= 0x0F) {
        // Short format
        serialized_tag[0] &= serialized_length[0];
    } else {
        // Long format
        serialized_tag[0] &= 0x10;
        serialized_tag[0] &= serialized_length.size();
        serialized_tag.insert(serialized_tag.begin(), serialized_length.begin(),
                              serialized_length.end());
    }

    return serialized_tag;
}

std::tuple<JsonTLVObject::Tag, size_t, JsonTLVObject::ByteArrayIterator>
JsonTLVObject::deserializeTagAndLength(const JsonTLVObject::ByteArray & bytes)
{
    // TODO: Maybe this should be an exception
    assert(!bytes.empty());

    Tag tag = static_cast<Tag>(bytes[0] >> 5);
    bool length_is_long_format = bytes[0] & ~0x1F;
    size_t length = bytes[0] & ~0xF0;
    JsonTLVObject::ByteArrayIterator next_it = bytes.begin() + 1;

    if (length_is_long_format) {
        // TODO: Maybe this should be an exception
        assert(bytes.size() >= length + 1);
        length = deserializeNumericValue<size_t>(next_it, next_it + length);
        next_it += length;
    }

    return std::make_tuple(tag, length, next_it);
}
