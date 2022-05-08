#include "json_packer.hpp"

#include <string>

#include <nlohmann/json.hpp>

void JsonPacker::pack(std::istream& in, std::ostream& out)
{
    JsonKeyDictionary dictionary;
    std::string line;

    while (std::getline(in, line)) {
        JsonPacker::packLine(line, dictionary);
    }
}

std::vector<uint8_t> JsonPacker::packLine(const std::string & line,
                                          JsonKeyDictionary & dictionary)
{
    auto json = nlohmann::json::parse(line); // TODO: Handle parse errors

    for (auto& element: json.items()) {
        int key_id = dictionary.insert(element.key());
    }

    return {};
}
