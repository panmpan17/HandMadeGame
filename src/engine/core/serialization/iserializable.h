#pragma once

#include <string>


class DataSerializer;
class DataDeserializer;

class ISerializable
{
public:
    ISerializable() = default;
    virtual ~ISerializable() = default;

    virtual void serializedTo(DataSerializer& serializer) const = 0;
    virtual bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) = 0;

    size_t getID() const { return m_nID; }

protected:
    size_t m_nID = 0;
};
