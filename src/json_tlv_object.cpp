#include "json_tlv_object.hpp"

#include "serialization.hpp"

bool JsonTLVObject::equalTo(const JsonTLVObject & other) const
{
    return this == &other;
}

JsonTLVObject::ByteArray JsonTLVObject::serializeTagAndLength(Tag tag,
                                                              size_t length)
{
    auto serialized_length = serializeIntegralValue(length);
    assert(serialized_length.size() <= 0x0F);
    auto serialized_tag = serializeIntegralValue(static_cast<uint8_t>(tag));
    assert(serialized_tag.size() == 1);

    serialized_tag[0] <<= 5;

    if (serialized_length.size() == 1 && serialized_length[0] <= 0x0F) {
        // Short format
        serialized_tag[0] |= serialized_length[0];
    } else {
        // Long format
        serialized_tag[0] |= 0x10;
        serialized_tag[0] |= serialized_length.size();
        serialized_tag.insert(serialized_tag.begin(), serialized_length.begin(),
                              serialized_length.end());
    }

    return serialized_tag;
}

std::tuple<JsonTLVObject::Tag, size_t, JsonTLVObject::ByteArrayIterator>
JsonTLVObject::deserializeTagAndLength(const JsonTLVObject::ByteArray & bytes)
{
    return JsonTLVObject::deserializeTagAndLength(bytes.begin(), bytes.end());
}

std::tuple<JsonTLVObject::Tag, size_t, JsonTLVObject::ByteArrayIterator>
JsonTLVObject::deserializeTagAndLength(ByteArrayIterator start,
                                       ByteArrayIterator end)
{
    // TODO: Maybe this should be an exception
    assert(end - start > 0);

    Tag tag = static_cast<Tag>(*start >> 5);
    bool length_is_long_format = *start & 0x10;
    size_t length = *start & 0x0F;
    auto next_it = start + 1;

    if (length_is_long_format) {
        // TODO: Maybe this should be an exception
        assert(end - start >= length + 1);
        length = deserializeIntegralValue<size_t>(next_it, next_it + length);
        next_it += length;
    }

    return std::make_tuple(tag, length, next_it);
}
