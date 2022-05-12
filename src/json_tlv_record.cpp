#include "json_tlv_record.hpp"
#include "serialization.hpp"

JsonTLVObject::ByteArray JsonTLVRecord::serialize() const
{
    JsonTLVObject::ByteArray bytes;

    for (auto& element: this->value) {
        auto key = element.first;
        auto& val = element.second;

        if (!val) {
            continue;
        }

        auto key_bytes = serializeTLVElement(JsonTLVInt(key));
        auto val_bytes = serializeTLVElement(*val);
        bytes.insert(bytes.end(), key_bytes.begin(), key_bytes.end());
        bytes.insert(bytes.end(), val_bytes.begin(), val_bytes.end());
    }

    return bytes;
}

void JsonTLVRecord::deserialize(
    const JsonTLVObject::ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVRecord::deserialize(
    JsonTLVObject::ByteArrayIterator start,
    JsonTLVObject::ByteArrayIterator end)
{
    // TODO: Assert that there is an even number of items (key-value)

    this->value.clear();
    std::shared_ptr<JsonTLVObject> key, val;

    while (start != end) {
        std::tie(key, start) = deserializeTLVElement(start, end);
        // TODO: Maybe this should be an exception
        assert(("First element must be the key",
                key->getTag() == Tag::Integer));
        std::tie(val, start) = deserializeTLVElement(start, end);
        this->value[dynamic_cast<JsonTLVInt&>(*key)] = std::move(val);
    }

    // TODO: Maybe this should be an exception
    assert(("The whole array [start, end) must have been read", start == end));
}

JsonTLVObject::Tag JsonTLVRecord::getTag() const
{
    return Tag::Record;
}

nlohmann::json JsonTLVRecord::toJson() const
{
    nlohmann::json json;

    for (auto & element: this->value) {
        json[element.first] = element.second->toJson();
    }

    return json;
}

Record::mapped_type & JsonTLVRecord::operator [](const Record::key_type & key)
{
    return this->value[key];
}
