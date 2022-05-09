#include <vector>
#include <cstdint>
#include <string>

#include <catch2/catch.hpp>

#include "json_packer.hpp"
#include "json_key_dictionary.hpp"

#include "json_tlv_integral.hpp"

TEST_CASE("JSON packer", "[json-packer]")
{
    SECTION("Should pack a single record")
    {
        JsonKeyDictionary dictionary;
        std::string record = "{\"key\": 2}";
        std::vector<uint8_t> expected_result{0x01, 0x21, 0x01, 0x21, 0x20};

        REQUIRE_THAT(JsonPacker::packLine(record, dictionary),
                     Catch::Matchers::Equals(expected_result));
    }
}
