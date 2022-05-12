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
    if (!in or !out or (in.flags() & std::ios_base::binary) or
        !(out.flags() & std::ios_base::binary)) {
        // TODO: This should the an exception
        assert(!in or !out or (in.flags() & std::ios_base::binary) or
               !(out.flags() & std::ios_base::binary));
    }

    JsonKeyDictionary dictionary;
    std::string line;

    while (std::getline(in, line)) {
        auto bytes = JsonPacker::packLine(line, dictionary);
        out.write(reinterpret_cast<char *>(bytes.data()), bytes.size());
    }

    JsonTLVRecord tlv_dictionary;

    for (auto & dict_pair: dictionary.getDictionary()) {
        tlv_dictionary[dict_pair.second] =
            std::make_shared<JsonTLVString>(dict_pair.first);
    }

    auto serialized_dictionary = serializeTLVElement(tlv_dictionary);
    out.write(reinterpret_cast<char *>(serialized_dictionary.data()),
              serialized_dictionary.size());
}

JsonTLVObject::ByteArray JsonPacker::packLine(const std::string & line,
                                              JsonKeyDictionary & dictionary)
{
    auto json = nlohmann::json::parse(line); // TODO: Handle parse errors
    JsonTLVRecord record;
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

        record[key_id] = std::move(tlv_element);
    }

    return serializeTLVElement(record);
}

void JsonPacker::unpack(std::istream& in, std::ostream& out)
{
    if (!in or !out or !(in.flags() & std::ios_base::binary) or
        (out.flags() & std::ios_base::binary)) {
        // TODO: This should the an exception
        assert(!in or !out or !(in.flags() & std::ios_base::binary) or
               (out.flags() & std::ios_base::binary));
    }


}

std::shared_ptr<JsonTLVObject> JsonPacker::unpackLine(
    JsonTLVObject::ByteArrayIterator start,
    JsonTLVObject::ByteArrayIterator end)
{
    return deserializeTLVElement(start, end).first;
}
