#include "serialization.hpp"
#include "json_tlv_integral.hpp"
#include "json_tlv_string.hpp"
#include "json_tlv_null.hpp"
#include "json_tlv_float.hpp"
#include "json_tlv_record.hpp"

ByteArray serializeTLVElement(const JsonTLVObject& tlv_value)
{
    auto value_bytes = tlv_value.serialize();
    auto tag_length_bytes = JsonTLVObject::serializeTagAndLength(
                                tlv_value.getTag(), value_bytes.size());

    tag_length_bytes.insert(tag_length_bytes.end(), value_bytes.begin(),
                            value_bytes.end());

    return tag_length_bytes;
}

void serializeTLVElement(const JsonTLVObject & tlv_value,
                         BinaryOutputStream & out)
{
    out.write(serializeTLVElement(tlv_value));
}

std::pair<std::shared_ptr<JsonTLVObject>, ByteArrayIterator>
deserializeTLVElement(ByteArrayIterator start, ByteArrayIterator end)
{
    JsonTLVObject::Tag tag;
    size_t length;
    std::shared_ptr<JsonTLVObject> tlv_value;

    std::tie(tag, length, start) = JsonTLVObject::deserializeTagAndLength(start,
                                                                          end);
    // TODO: Maybe this should be an exception
    assert(end - start >= length);
    end = start + length;

    switch (tag) {
        case JsonTLVObject::Tag::Record:
            tlv_value = std::make_shared<JsonTLVRecord>();
            break;

        case JsonTLVObject::Tag::Integer:
            tlv_value = std::make_shared<JsonTLVInt>();
            break;

        case JsonTLVObject::Tag::Boolean:
            tlv_value = std::make_shared<JsonTLVBoolean>();
            break;

        case JsonTLVObject::Tag::String:
            tlv_value = std::make_shared<JsonTLVString>();
            break;

        case JsonTLVObject::Tag::Null:
            tlv_value = std::make_shared<JsonTLVNull>();
            break;

        case JsonTLVObject::Tag::Float:
            tlv_value = std::make_shared<JsonTLVFloat>();
            break;

        // TODO: Do nothing in default case after all tags are implemented
        default:
            assert(("Invalid tag found", false));
            break;
    }

    tlv_value->deserialize(start, end);

    return std::make_pair(tlv_value, end);
}

std::shared_ptr<JsonTLVObject> deserializeTLVElement(BinaryInputStream & in)
{
    JsonTLVObject::Tag tag;
    size_t length;
    std::shared_ptr<JsonTLVObject> tlv_value;

    std::tie(tag, length) = JsonTLVObject::deserializeTagAndLength(in);

    switch (tag) {
        case JsonTLVObject::Tag::Record:
            tlv_value = std::make_shared<JsonTLVRecord>();
            break;

        case JsonTLVObject::Tag::Integer:
            tlv_value = std::make_shared<JsonTLVInt>();
            break;

        case JsonTLVObject::Tag::Boolean:
            tlv_value = std::make_shared<JsonTLVBoolean>();
            break;

        case JsonTLVObject::Tag::String:
            tlv_value = std::make_shared<JsonTLVString>();
            break;

        case JsonTLVObject::Tag::Null:
            tlv_value = std::make_shared<JsonTLVNull>();
            break;

        case JsonTLVObject::Tag::Float:
            tlv_value = std::make_shared<JsonTLVFloat>();
            break;

        // TODO: Do nothing in default case after all tags are implemented
        default:
            assert(("Invalid tag found", false));
            break;
    }

    tlv_value->deserialize(in.read(length));

    return tlv_value;
}
