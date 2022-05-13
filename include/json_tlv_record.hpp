#ifndef JSON_TLV_RECORD_HPP
#define JSON_TLV_RECORD_HPP

#include <vector>
#include <map>
#include <memory>
#include <utility>

#include <nlohmann/json.hpp>

#include "json_tlv_object.hpp"
#include "json_tlv_integral.hpp"
#include "binary_stream.hpp"

using Record = std::map<long long, std::shared_ptr<JsonTLVObject>>;

struct RecordComp
{
    bool operator()(const Record & lhs, const Record & rhs)
    {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (auto lhs_it = lhs.begin(), rhs_it = rhs.begin();
             lhs_it != lhs.end(); lhs_it++, rhs_it++) {
            if (lhs_it->first != rhs_it->first ||
                !lhs_it->second->equalTo(*lhs_it->second)) {
                return false;
            }
        }

        return true;
    };
};

class JsonTLVRecord: public JsonTLVValueHolder<Record, RecordComp>
{
public:

    using JsonTLVValueHolder::JsonTLVValueHolder;

    ByteArray serialize() const override;

    void deserialize(const ByteArray & bytes) override;

    void deserialize(ByteArrayIterator start, ByteArrayIterator end) override;

    Tag getTag() const override;

    nlohmann::json toJson() const override;

    Record::mapped_type & operator [](const Record::key_type & key);
};

#endif // JSON_TLV_RECORD_HPP
