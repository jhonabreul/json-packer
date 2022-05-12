#ifndef JSON_KEY_DICTIONARY_HPP
#define JSON_KEY_DICTIONARY_HPP

#include <unordered_map>
#include <string>

class JsonKeyDictionary
{
public:

    using Dictionary = std::unordered_map<std::string, int>;

    int insert(const std::string& key);

    int get(const std::string& key) const;

    const Dictionary & getDictionary() const;

private:

    Dictionary dictionary;

    int next_id = 1;
};

#endif // JSON_KEY_DICTIONARY_HPP
