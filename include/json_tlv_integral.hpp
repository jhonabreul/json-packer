#ifndef JSON_TLV_INTEGRAL_HPP
#define JSON_TLV_INTEGRAL_HPP

#include <vector>
#include <cstdint>
#include <type_traits>

#include <nlohmann/json.hpp>

#include "json_tlv_object.hpp"
#include "serialization.hpp"
#include "binary_stream.hpp"

template<
    class T,
    typename = typename std::enable_if_t<std::is_integral<T>::value, T>
>
class JsonTLVIntegral: public JsonTLVValueHolder<T>
{
public:

    using JsonTLVValueHolder<T>::JsonTLVValueHolder;

    ByteArray serialize() const override;

    void deserialize(const ByteArray & bytes) override;

    void deserialize(ByteArrayIterator start, ByteArrayIterator end) override;

    JsonTLVObject::Tag getTag() const override;

    nlohmann::json toJson() const override;

protected:

    struct TagSelector
    {
        static constexpr JsonTLVObject::Tag getTag()
        {
            if (std::is_same<T, bool>::value) {
                return JsonTLVObject::Tag::Boolean;
            }

            if (std::is_integral<T>::value) {
                return JsonTLVObject::Tag::Integer;
            }

            return JsonTLVObject::Tag::Float;
        }
    };

    static constexpr JsonTLVObject::Tag tag = TagSelector::getTag();
};

template<class T, typename P>
JsonTLVObject::Tag JsonTLVIntegral<T, P>::getTag() const
{
    return JsonTLVIntegral::tag;
}

template<class T, typename P>
ByteArray JsonTLVIntegral<T, P>::serialize() const
{
    return this->value == 0 ? ByteArray() : serializeIntegralValue(this->value);
}

template<class T, typename P>
void JsonTLVIntegral<T, P>::deserialize(const ByteArray & bytes)
{
    this->value = bytes.empty() ? 0 : deserializeIntegralValue<T>(bytes);
}

template<class T, typename P>
void JsonTLVIntegral<T, P>::deserialize(ByteArrayIterator start,
                                        ByteArrayIterator end)
{
    this->value = deserializeIntegralValue<T>(start, end);
}

template<class T, typename P>
nlohmann::json JsonTLVIntegral<T, P>::toJson() const
{
    return nlohmann::json(this->value);
}

template<class T, typename P>
constexpr JsonTLVObject::Tag JsonTLVIntegral<T, P>::tag;

#define DECLARE_TLV_NUMERIC_CLASS(name, type) \
    class name: public JsonTLVIntegral<type> \
    { \
    public: \
        using JsonTLVIntegral<type>::JsonTLVIntegral; \
    };

DECLARE_TLV_NUMERIC_CLASS(JsonTLVInt, long long);
DECLARE_TLV_NUMERIC_CLASS(JsonTLVBoolean, bool);

#endif // JSON_TLV_INTEGRAL_HPP
