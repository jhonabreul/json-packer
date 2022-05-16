#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include "json_packer.hpp"
#include "json_key_dictionary.hpp"
#include "json_tlv_integral.hpp"
#include "json_tlv_string.hpp"
#include "json_tlv_null.hpp"
#include "json_tlv_float.hpp"
#include "json_tlv_record.hpp"

template<class T>
bool compareJsonToTLVRecord(const nlohmann::json& json_value,
                            const JsonTLVObject& record_value)
{
    using ValueType = typename T::ValueType;

    return (json_value.get<ValueType>() ==
        dynamic_cast<const T&>(record_value).getValue());
}

TEST_CASE("JSON packer", "[json-packer]")
{
    SECTION("Working with single records")
    {
        auto test_pair = GENERATE(
            std::make_pair(
                "{\"key1\": false,"
                " \"key2\": 0,"
                " \"key3\": true,"
                " \"key4\": 112394521950,"
                " \"key5\": null,"
                " \"key6\": \"string\"}",
                /*
                 * NOTE: The order is expected to be the same because nlohmann::json
                 *       uses an std::map to store the json object, so the keys are
                 *       sorted in alphabetical order.
                 */
                std::vector<uint8_t>{
                    0x11, // START: Record tag-length byte(s)
                    0x1E, // END: Record tag-length byte(s)
                    0x21, // START: key1 key (int 1)
                    0x01, // END: key1 key (int 1)
                    0x60, // START-END: key1 value (bool false)
                    0x21, // START: key2 key (int 2)
                    0x02, // END: key2 key (int 2)
                    0x20, // START-END: key2 value (int 0)
                    0x21, // START: key3 key (int 3)
                    0x03, // END: key3 key (int 3)
                    0x61, // START: key3 value (bool true)
                    0x01, // END: key3 value (bool true)
                    0x21, // START: key4 key (int 4)
                    0x04, // END: key4 key (int 4)
                    0x25, // START: key4 value (int 112394521950 = 0x1A2B3C4D5E)
                    0x5E,
                    0x4D,
                    0x3C,
                    0x2B,
                    0x1A, // END: key4 value (int 112394521950 = 0x1A2B3C4D5E)
                    0x21, // START: key5 key (int 5)
                    0x05, // END: key5 key (int 5)
                    0xA0, // START-END: key5 value (null)
                    0x21, // START: key6 key (int 6)
                    0x06, // END: key6 key (int 6)
                    0x86, // START: key6 value (string "string")
                    's',
                    't',
                    'r',
                    'i',
                    'n',
                    'g', // END: key6 value (string "string")
                }));

        SECTION("Should pack a single record")
        {
            JsonKeyDictionary dictionary;
            auto record = test_pair.first;
            auto & expected_result = test_pair.second;

            REQUIRE_THAT(JsonPacker::packLine(record, dictionary),
                         Catch::Matchers::Equals(expected_result));
        }

        SECTION("Should unpack a single record")
        {
            auto json_record = nlohmann::json::parse(test_pair.first);
            auto& serialized_record = test_pair.second;
            auto record = JsonPacker::unpackLine(serialized_record.begin(),
                                                 serialized_record.end());

            REQUIRE(record->getTag() == JsonTLVObject::Tag::Record);

            auto& record_map = dynamic_cast<JsonTLVRecord&>(*record).getValue();
            auto record_it = record_map.begin();

            REQUIRE(record_map.size() == json_record.size());

            REQUIRE_THAT(
                record_map,
                Catch::Matchers::Predicate<JsonTLVRecord::ValueType>(
                    [&] (const JsonTLVRecord::ValueType & tlv_record_map) -> bool {
                        for (auto& json_entry: json_record.items()) {
                            auto& json_value = json_entry.value();
                            auto& record_value = (record_it++)->second;

                            switch (record_value->getTag()) {
                                case JsonTLVObject::Tag::Integer:
                                    return compareJsonToTLVRecord<JsonTLVInt>(
                                        json_value, *record_value);

                                case JsonTLVObject::Tag::Boolean:
                                    return compareJsonToTLVRecord<JsonTLVBoolean>(
                                        json_value, *record_value);

                                case JsonTLVObject::Tag::String:
                                    return compareJsonToTLVRecord<JsonTLVString>(
                                        json_value, *record_value);

                                case JsonTLVObject::Tag::Null:
                                    return compareJsonToTLVRecord<JsonTLVNull>(
                                        json_value, *record_value);

                                case JsonTLVObject::Tag::Float:
                                    return compareJsonToTLVRecord<JsonTLVFloat>(
                                        json_value, *record_value);

                                default:
                                    break;
                            }
                        }

                        return false;
                    },
                    "Record and JSON item values must be the same"));
        }

        SECTION("Should pack and unpack a file")
        {
            const std::string binary_out_filename = "test_data.bin";

            std::stringstream original_in(
                "{\"key1\": false, \"key2\": 0, \"key3\": true, \"key4\": 112394521950, \"key5\": null, \"key6\": \"string\", \"key7\": 251.22336, \"key8\": 0.0}\n"
                "{\"key1\": true, \"key2\": 56, \"key3\": null, \"key4\": false, \"key5\": \"HOLA gfjngt kifdg dkjgnm mf gkfg\", \"key6\": 0.000005, \"key7\": 251.22336, \"key8\": 88888888.0}\n");
            std::ofstream binary_packed_out(binary_out_filename,
                                            std::ios::binary);

            REQUIRE(original_in);
            REQUIRE(binary_packed_out);

            JsonPacker::pack(original_in, binary_packed_out);
            binary_packed_out.close();

            std::ifstream binary_packed_in(binary_out_filename,
                                           std::ios::binary);
            std::stringstream unpacked_stream;

            REQUIRE(binary_packed_in);
            REQUIRE(binary_packed_out);

            JsonPacker::unpack(binary_packed_in, unpacked_stream);
            binary_packed_in.close();
            unpacked_stream.clear();
            unpacked_stream.seekg(0);
            original_in.clear();
            original_in.seekg(0);

            std::string original_line, unpacked_line;
            int i = 1;

            REQUIRE(original_in);
            REQUIRE(unpacked_stream);

            while (true) {
                std::getline(original_in, original_line);
                std::getline(unpacked_stream, unpacked_line);

                if (!original_in || !unpacked_stream) {
                    break;
                }

                SECTION("Records should match for line " + std::to_string(i++))
                {
                    REQUIRE(nlohmann::json::parse(original_line) ==
                                nlohmann::json::parse(unpacked_line));
                }
            }

            REQUIRE(!original_in);
            REQUIRE(!unpacked_stream);
            std::remove(binary_out_filename.c_str());
        }
    }
}
