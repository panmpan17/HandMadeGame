#pragma once

#include <string>
#include <unordered_map>


enum class PreferenceType : int
{
    None = 0,
    Bool = 1,
    Int = 2,
};

class PlayerPreference
{
public:
    PlayerPreference(const std::string& strFilename);
    ~PlayerPreference();

    inline bool getBool(const std::string& strKey, bool defaultValue)
    {
        auto it = m_mapBoolPreferences.find(strKey);
        if (it != m_mapBoolPreferences.end())
        {
            return it->second;
        }
        return defaultValue;
    }
    inline void setBool(const std::string& strKey, bool value) { m_mapBoolPreferences[strKey] = value; }

    inline int getInt(const std::string& strKey, int defaultValue)
    {
        auto it = m_mapIntPreferences.find(strKey);
        if (it != m_mapIntPreferences.end())
        {
            return it->second;
        }
        return defaultValue;
    }
    inline void setInt(const std::string& strKey, int value) { m_mapIntPreferences[strKey] = value; }


    void writeToFile();

private:
    std::string m_strFilename;

    std::unordered_map<std::string, bool> m_mapBoolPreferences;
    std::unordered_map<std::string, int> m_mapIntPreferences;

    void readFromFile();
};
