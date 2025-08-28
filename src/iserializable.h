#pragma once

#include <string>

class DataSerializer;

class ISerializable
{
public:
    ISerializable() = default;
    virtual ~ISerializable() = default;

    virtual void serializedTo(DataSerializer& serializer) const = 0;
    virtual void deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue) = 0;
};