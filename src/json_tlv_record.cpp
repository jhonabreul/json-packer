#include <exception>

#include "json_tlv_record.hpp"
#include "serialization.hpp"

ByteArray JsonTLVRecord::serialize() const
{
    ByteArray bytes;

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

void JsonTLVRecord::deserialize(const ByteArray & bytes)
{
    return this->deserialize(bytes.begin(), bytes.end());
}

void JsonTLVRecord::deserialize(ByteArrayIterator start, ByteArrayIterator end)
{
    this->value.clear();
    const auto length = end - start;
    std::shared_ptr<JsonTLVObject> key, val;

    while (start != end) {
        std::tie(key, start) = deserializeTLVElement(start, end);

        if (key->getTag() != Tag::Integer) {
            throw std::domain_error("The first element of a packed record "
                                    "value must be an integer (the key)");
        }

        std::tie(val, start) = deserializeTLVElement(start, end);
        this->value[dynamic_cast<JsonTLVInt&>(*key)] = std::move(val);
    }

    if (start != end) {
        throw std::domain_error(
            std::to_string(length) +
            " bytes were received to deserialize a record but only " +
            std::to_string(length - (end  - start)) +
            " were read");
    }
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
