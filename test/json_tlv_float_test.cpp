#include <catch2/catch.hpp>

#include <nlohmann/json.hpp>

#include "json_tlv_float.hpp"
#include "json_tlv_object.hpp"

TEST_CASE("JSON TLV float", "[tlv-float]")
{
    SECTION("Tag should be 'Float'")
    {
        REQUIRE(JsonTLVFloat().getTag() == JsonTLVObject::Tag::Float);
    }

    auto test_pair = GENERATE(
        std::make_pair(JsonTLVFloat(0.0), nlohmann::json::to_cbor(0.0)),
        std::make_pair(JsonTLVFloat(1.0), nlohmann::json::to_cbor(1.0)),
        std::make_pair(JsonTLVFloat(0.0001), nlohmann::json::to_cbor(0.0001)),
        std::make_pair(JsonTLVFloat(12345.6789),
                       nlohmann::json::to_cbor(12345.6789)),
        std::make_pair(JsonTLVFloat(0.000000123),
                       nlohmann::json::to_cbor(0.000000123)));

    SECTION("Should serialize floating point values")
    {
        auto & tlv_float = test_pair.first;
        auto & expected_serialization = test_pair.second;

        REQUIRE_THAT(tlv_float.serialize(),
                     Catch::Matchers::Equals(expected_serialization));
    }

    SECTION("Should deserialize floating point values")
    {
        auto & expected_tlv_float = test_pair.first;
        auto & serialization = test_pair.second;

        JsonTLVFloat deserialized_value;
        deserialized_value.deserialize(serialization);

        REQUIRE(deserialized_value.equalTo(expected_tlv_float));
    }
}
