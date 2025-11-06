#include "player_preference.h"
#include "file_utils.h"
#include "string_handle.h"


PlayerPreference::PlayerPreference(const std::string& strFilename)
    : m_strFilename(strFilename)
{
    readFromFile();
}

PlayerPreference::~PlayerPreference()
{
}

void PlayerPreference::readFromFile()
{
    FileReader reader(m_strFilename);
    if (!reader.isOpen())
    {
        return;
    }

    std::string strLine;
    while (reader.readLine(strLine))
    {
        std::vector<std::string> vetKeyValue = splitString(strLine, ',');
        if (vetKeyValue.size() != 3)
        {
            continue;
        }

        const std::string& strKey = vetKeyValue[0];
        PreferenceType eType = static_cast<PreferenceType>(atoi(vetKeyValue[1].c_str()));
        const std::string& strValue = vetKeyValue[2];

        switch (eType)
        {
            case PreferenceType::Bool:
                m_mapBoolPreferences[strKey] = strValue == "1";
                break;
            case PreferenceType::Int:
                m_mapIntPreferences[strKey] = atoi(strValue.c_str());
                break;
            default:
                break;
        }
    }
}

void PlayerPreference::writeToFile()
{
    FileWriter writer(m_strFilename, false);
    if (!writer.isOpen())
    {
        return;
    }

    for (const auto& pair : m_mapBoolPreferences)
    {
        writer << pair.first << ',' << static_cast<int>(PreferenceType::Bool) << ',' << (pair.second ? '1' : '0') << "\n";
    }

    for (const auto& pair : m_mapIntPreferences)
    {
        writer << pair.first << ',' << static_cast<int>(PreferenceType::Int) << ',' << pair.second << "\n";
    }
}
