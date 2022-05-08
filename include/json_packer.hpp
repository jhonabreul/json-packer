#ifndef JSON_PACKER_HPP
#define JSON_PACKER_HPP

#include <unordered_map>
#include <vector>
#include <istream>
#include <cstdint>

#include "json_key_dictionary.hpp"

class JsonPacker
{
public:

    static void pack(std::istream& in, std::ostream& out);

    static std::vector<uint8_t> packLine(const std::string & line,
                                         JsonKeyDictionary & dictionary);
};

#endif // JSON_PACKER_HPP
