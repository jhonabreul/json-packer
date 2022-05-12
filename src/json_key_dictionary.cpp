#include "json_key_dictionary.hpp"

int JsonKeyDictionary::insert(const std::string& key)
{
    auto& id = this->dictionary[key];

    if (id == 0) {
        id = this->next_id++;
    }

    return id;
}

int JsonKeyDictionary::get(const std::string& key) const
{
    return this->dictionary.at(key);
}

const JsonKeyDictionary::Dictionary & JsonKeyDictionary::getDictionary() const
{
    return this->dictionary;
}
