#include <vector>
#include <utility>
#include <memory>

#include <catch2/catch.hpp>

#include "json_tlv_string.hpp"
#include "json_tlv_object.hpp"

TEST_CASE("JSON TLV string", "[tlv-string]")
{
    SECTION("Tag should be 'String'")
    {
        REQUIRE(JsonTLVString().getTag() == JsonTLVObject::Tag::String);
    }

    SECTION("Should serialize \"\" to empty array")
    {
        REQUIRE_THAT(JsonTLVString("").serialize(),
                     Catch::Matchers::Equals(std::vector<uint8_t>()));
    }

    SECTION("Should deserialize empty array to \"\"")
    {
        JsonTLVString deserialized_value("not empty");
        REQUIRE_FALSE(deserialized_value.getValue().empty());

        deserialized_value.deserialize({});
        REQUIRE(deserialized_value.getValue().empty());
    }

    auto test_pair = GENERATE(
        std::make_pair(JsonTLVString("english"),
                       std::vector<uint8_t>{'e', 'n', 'g', 'l', 'i', 's', 'h'}),
        std::make_pair(
            // UTF-8
            JsonTLVString("español"),
            std::vector<uint8_t>{'e', 's', 'p', 'a', 0xC3, 0xB1, 'o', 'l'})
    );

    SECTION("Should serialize string")
    {
        auto & tlv_string = test_pair.first;
        auto & expected_serialization = test_pair.second;

        REQUIRE_THAT(tlv_string.serialize(),
                     Catch::Matchers::Equals(expected_serialization));
    }

    SECTION("Should deserialize string")
    {
        auto & expected_tlv_string = test_pair.first;
        auto & serialization = test_pair.second;

        JsonTLVString deserialized_value;
        deserialized_value.deserialize(serialization);

        REQUIRE(deserialized_value.equalTo(expected_tlv_string));
    }
}
