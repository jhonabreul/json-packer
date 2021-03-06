#include <exception>

#include "json_tlv_object.hpp"
#include "serialization.hpp"

bool JsonTLVObject::equalTo(const JsonTLVObject & other) const
{
    return this == &other;
}

ByteArray JsonTLVObject::serializeTagAndLength(Tag tag, size_t length)
{
    auto serialized_length = serializeIntegralValue(length);

    if (serialized_length.size() > 0x0F) {
        throw std::domain_error("Element length cannot be serialized");
    }

    auto serialized_tag = serializeIntegralValue(static_cast<uint8_t>(tag));

    if (serialized_tag.size() != 1) {
        throw std::domain_error(
            "Tag size must be one byte, could not be serialized");
    }

    serialized_tag[0] <<= 5;

    if (serialized_length.size() == 1 && serialized_length[0] <= 0x0F) {
        // Short format
        serialized_tag[0] |= serialized_length[0];
    } else {
        // Long format
        serialized_tag[0] |= 0x10;
        serialized_tag[0] |= serialized_length.size();
        serialized_tag.insert(serialized_tag.end(), serialized_length.begin(),
                              serialized_length.end());
    }

    return serialized_tag;
}

void JsonTLVObject::serializeTagAndLength(Tag tag, size_t length,
                                          BinaryOutputStream & out)
{
    out.write(serializeTagAndLength(tag, length));
}

std::tuple<JsonTLVObject::Tag, size_t, ByteArrayIterator>
JsonTLVObject::deserializeTagAndLength(const ByteArray & bytes)
{
    return JsonTLVObject::deserializeTagAndLength(bytes.begin(), bytes.end());
}

std::tuple<JsonTLVObject::Tag, size_t, ByteArrayIterator>
JsonTLVObject::deserializeTagAndLength(ByteArrayIterator start,
                                       ByteArrayIterator end)
{
    if (start == end) {
        throw std::length_error("Not enought bytes to deserialize tag and length");
    }

    Tag tag = static_cast<Tag>(*start >> 5);
    bool length_is_long_format = *start & 0x10;
    size_t length = *start & 0x0F;
    auto next_it = ++start;

    if (length_is_long_format) {
        if (end - start < length + 1) {
            throw std::length_error("Not enought bytes to deserialize length");
        }

        start += length;
        length = deserializeIntegralValue<size_t>(next_it, next_it + length);
    }

    return std::make_tuple(tag, length, start);
}

std::pair<JsonTLVObject::Tag, size_t> JsonTLVObject::deserializeTagAndLength(
    BinaryInputStream & in)
{
    ByteArray bytes = in.read(sizeof(Byte));
    Tag tag = static_cast<Tag>(bytes[0] >> 5);
    bool length_is_long_format = bytes[0] & 0x10;
    size_t length = bytes[0] & 0x0F;

    if (length_is_long_format) {
        length = deserializeIntegralValue<size_t>(in.read(length * sizeof(Byte)));
    }

    return std::make_pair(tag, length);
}
