#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include "json_packer.hpp"
#include "json_key_dictionary.hpp"
#include "json_tlv_record.hpp"

TEST_CASE("JSON packer", "[json-packer]")
{
    SECTION("Working with single records")
    {
        auto test_pair = GENERATE(
            std::make_pair(
                "{\"key1\": false,"
                " \"key2\": 0,"
                " \"key3\": true,"
                " \"key4\": 112394521950}",
                /*
                 * NOTE: The order is expected to be the same because nlohmann::json
                 *       uses an std::map to store the json object, so the keys are
                 *       sorted in alphabetical order.
                 */
                std::vector<uint8_t>{
                    0x11, // START: Record tag-length byte(s)
                    0x12, // END: Record tag-length byte(s)
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

            auto& record_map =
                static_cast<const JsonTLVRecord::ValueType&>(
                    dynamic_cast<JsonTLVRecord&>(*record));
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
                                    return (json_value.get<JsonTLVInt::ValueType>() ==
                                        dynamic_cast<JsonTLVInt&>(*record_value));

                                case JsonTLVObject::Tag::Boolean:
                                    return (
                                        json_value.get<JsonTLVBoolean::ValueType>() ==
                                            dynamic_cast<JsonTLVBoolean&>(*record_value));

                                // TODO: Do nothing in default case after all tags are
                                //       implemented
                                default:
                                    // assert(("Unimplemented tag", false));
                                    break;
                            }
                        }

                        return false;
                    },
                    "Record and JSON item values must be the same"));
        }
    }
}
