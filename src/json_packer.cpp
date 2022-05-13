#include "json_packer.hpp"

#include <string>
#include <memory>
#include <utility>
#include <list>

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

    BinaryOutputStream output(out);
    JsonKeyDictionary dictionary;
    std::string line;

    while (std::getline(in, line)) {
        output.write(JsonPacker::packLine(line, dictionary));
    }

    JsonTLVRecord tlv_dictionary;

    for (auto & dict_pair: dictionary.getDictionary()) {
        tlv_dictionary[dict_pair.second] =
            std::make_shared<JsonTLVString>(dict_pair.first);
    }

    output.write(serializeTLVElement(tlv_dictionary));
}

ByteArray JsonPacker::packLine(const std::string & line,
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

nlohmann::json recordToJson(const JsonTLVRecord & record,
                            JsonTLVRecord & dictionary)
{
    nlohmann::json json_record;
    auto & record_map = record.getValue();

    for (auto & item: record_map) {
        auto id = item.first;
        auto & key = dynamic_cast<JsonTLVString &>(*dictionary[id]).getValue();

        switch (item.second->getTag()) {
            case JsonTLVObject::Tag::Integer:
                json_record[key] =
                    dynamic_cast<JsonTLVInt &>(*item.second).getValue();
                break;

            case JsonTLVObject::Tag::Boolean:
                json_record[key] =
                    dynamic_cast<JsonTLVBoolean &>(*item.second).getValue();
                break;

            case JsonTLVObject::Tag::String:
                json_record[key] =
                    dynamic_cast<JsonTLVString &>(*item.second).getValue();
                break;

            case JsonTLVObject::Tag::Null:
                json_record[key] =
                    dynamic_cast<JsonTLVNull &>(*item.second).getValue();
                break;

            case JsonTLVObject::Tag::Float:
                json_record[key] =
                    dynamic_cast<JsonTLVFloat &>(*item.second).getValue();
                break;

            // TODO: Do nothing in default case after all tags are implemented
            default:
                assert(("Invalid tag found", false));
                break;
        }
    }

    return json_record;
}

nlohmann::json dictionaryToJson(JsonTLVRecord & dictionary)
{
    nlohmann::json json_record;

    for (auto & item: dictionary.getValue()) {
        auto id = item.first;
        auto & key = dynamic_cast<JsonTLVString &>(*dictionary[id]).getValue();
        json_record[key] =
            dynamic_cast<JsonTLVString &>(*item.second).getValue();
    }

    return json_record;
}

void JsonPacker::unpack(std::istream& in, std::ostream& out)
{
    if (!in or !out or !(in.flags() & std::ios_base::binary) or
        (out.flags() & std::ios_base::binary)) {
        // TODO: This should the an exception
        assert(!in or !out or !(in.flags() & std::ios_base::binary) or
               (out.flags() & std::ios_base::binary));
    }

    std::list<std::shared_ptr<JsonTLVObject>> tlv_elements;

    BinaryInputStream input(in);
    in.seekg(0, in.end);
    auto length = in.tellg();
    in.seekg (0, in.beg);

    while (!in or in.tellg() < length) {
        try {
            tlv_elements.push_back(deserializeTLVElement(input));
        } catch (std::exception & e) {
            // TODO: Break more gracefully
            assert(("File is malformed", false));
        }
    }

    if (tlv_elements.size() == 1) {
        // TODO: Break more gracefully
        assert(("File is incomplete, no dictionary found", false));
    }

    try {
        auto tlv_dictionary =
            dynamic_cast<JsonTLVRecord&>(*tlv_elements.back());
        tlv_elements.pop_back();

        for (auto & tlv_element: tlv_elements) {
            out << recordToJson(dynamic_cast<JsonTLVRecord&>(*tlv_element),
                                tlv_dictionary)
                << '\n';
        }

        // out << dictionaryToJson(tlv_dictionary) << '\n';
    } catch (std::bad_cast) {
        assert(false);
    }
}

std::shared_ptr<JsonTLVObject> JsonPacker::unpackLine(ByteArrayIterator start,
                                                      ByteArrayIterator end)
{
    return deserializeTLVElement(start, end).first;
}
