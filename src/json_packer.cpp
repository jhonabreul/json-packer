#include "json_packer.hpp"

#include <string>
#include <memory>
#include <utility>

#include <nlohmann/json.hpp>

#include "json_tlv_object.hpp"
#include "json_tlv_integral.hpp"
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
        } else {
            assert(("Unimplemented data type", false));
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
