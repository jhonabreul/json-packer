#include "json_packer.hpp"

#include <string>
#include <memory>
#include <utility>

#include <nlohmann/json.hpp>

#include "json_tlv_object.hpp"
#include "json_tlv_integral.hpp"
#include "json_tlv_string.hpp"
#include "json_tlv_null.hpp"
#include "json_tlv_float.hpp"
#include "json_tlv_record.hpp"
#include "serialization.hpp"

void JsonPacker::pack(std::istream& in, std::ostream& out)
{
    JsonKeyDictionary dictionary;
    std::string line;

    while (std::getline(in, line)) {
        JsonPacker::packLine(line, dictionary);
    }
}

JsonTLVObject::ByteArray JsonPacker::packLine(const std::string & line,
                                              JsonKeyDictionary & dictionary)
{
    auto json = nlohmann::json::parse(line); // TODO: Handle parse errors
    JsonTLVRecord::ValueType record_map;
    std::shared_ptr<JsonTLVObject> tlv_element;

    for (auto& element: json.items()) {
        int key_id = dictionary.insert(element.key());
        auto & value = element.value();

        if (value.is_number_integer()) {
            tlv_element = std::make_shared<JsonTLVInt>(
                            value.get<JsonTLVInt::ValueType>());
        } else if (value.is_boolean()) {
            tlv_element = std::make_shared<JsonTLVBoolean>(
                            value.get<JsonTLVBoolean::ValueType>());
        } else if (value.is_string()) {
            tlv_element = std::make_shared<JsonTLVString>(
                            value.get<JsonTLVString::ValueType>());
        } else if (value.is_null()) {
            tlv_element = std::make_shared<JsonTLVNull>();
        } else if (value.is_number_float()) {
            tlv_element = std::make_shared<JsonTLVFloat>(
                            value.get<JsonTLVFloat::ValueType>());
        } else {
            assert(("A JSON record line can only contain simple type values",
                    false));
        }

        record_map.emplace(key_id, std::move(tlv_element));
    }

    return serializeTLVElement(JsonTLVRecord(std::move(record_map)));
}

std::shared_ptr<JsonTLVObject> JsonPacker::unpackLine(
    JsonTLVObject::ByteArrayIterator start,
    JsonTLVObject::ByteArrayIterator end)
{
    return deserializeTLVElement(start, end).first;
}
