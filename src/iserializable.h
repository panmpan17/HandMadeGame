#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
// #include "Base.h"

class DataSerializer;

class ISerializable
{
public:
    ISerializable() = default;
    virtual ~ISerializable() = default;

    virtual void serializedTo(DataSerializer& serializer) const = 0;
    virtual bool deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue) = 0;
};


class TypeRegistry {
public:
    using Creator = std::function<ISerializable*()>;

    static TypeRegistry& instance()
    {
        static TypeRegistry instance;
        return instance;
    }

    void registerType(const std::string& strTypeName, Creator funcCreator)
    {
        auto it = m_mapRegistry.find(strTypeName);
        if (it == m_mapRegistry.end())
        {
            m_mapRegistry[strTypeName] = funcCreator;
        }
    }

    ISerializable* create(const std::string& strTypeName) const
    {
        auto it = m_mapRegistry.find(strTypeName);
        if (it != m_mapRegistry.end())
        {
            return it->second();
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, Creator> m_mapRegistry;
};

#define REGISTER_CLASS(T) \
    namespace { \
        struct T##_Registrator { \
            T##_Registrator() { \
                TypeRegistry::instance().registerType(#T, []() -> ISerializable* { return new T(); }); \
            } \
        }; \
        static T##_Registrator global_##T##_registrator; \
    }
