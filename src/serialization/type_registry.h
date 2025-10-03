#pragma once

#include <functional>

class ISerializable;


struct TypeInfo
{
    size_t m_nTypeId;
    std::string m_strTypeName;
    std::string m_strTitlizedName;
    std::function<ISerializable*()> m_funcCreator;

    TypeInfo(size_t nId, const std::string& strName, const std::string& strTitlizedName, std::function<ISerializable*()> funcCreator = nullptr)
        : m_nTypeId(nId), m_strTypeName(strName), m_strTitlizedName(strTitlizedName), m_funcCreator(funcCreator) {}
};


class TypeRegistry {
public:
    using Creator = std::function<ISerializable*()>;

    static std::string titlizeTypeIdName(const std::string& strName);

    static TypeRegistry& instance()
    {
        static TypeRegistry instance;
        return instance;
    }

    void registerType(const std::string& strTypeName, const TypeInfo& typeInfo)
    {
        auto it = std::find_if(m_vecTypeInfos.begin(), m_vecTypeInfos.end(),
            [&strTypeName](const TypeInfo& info) { return info.m_strTypeName == strTypeName; });

        if (it == m_vecTypeInfos.end())
        {
            m_vecTypeInfos.push_back(typeInfo);
        }
    }

    ISerializable* create(const std::string& strTypeName) const
    {
        auto it = std::find_if(m_vecTypeInfos.begin(), m_vecTypeInfos.end(),
            [&strTypeName](const TypeInfo& info) { return info.m_strTypeName == strTypeName; });

        if (it != m_vecTypeInfos.end())
        {
            return it->m_funcCreator();
        }
        return nullptr;
    }

    const std::string& getTitlizedName(const std::type_info& oTypeInfo) const
    {
        auto it = std::find_if(m_vecTypeInfos.begin(), m_vecTypeInfos.end(),
            [&oTypeInfo](const TypeInfo& info) { return info.m_nTypeId == oTypeInfo.hash_code(); });

        if (it != m_vecTypeInfos.end())
        {
            return it->m_strTitlizedName;
        }

        static std::string tempStr;
        tempStr = titlizeTypeIdName(oTypeInfo.name());
        return tempStr;
    }

private:
    std::vector<TypeInfo> m_vecTypeInfos;
};

#define REGISTER_CLASS(T) \
    namespace { \
        struct T##_Registrator { \
            T##_Registrator() { \
                TypeInfo oInfo(typeid(T).hash_code(), #T, TypeRegistry::titlizeTypeIdName(#T), []() -> ISerializable* { return new T(); }); \
                TypeRegistry::instance().registerType(#T, oInfo); \
            } \
        }; \
        static T##_Registrator global_##T##_registrator; \
    }
