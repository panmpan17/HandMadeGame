#include "player_preference.h"
#include "file_utils.h"


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
        size_t nDelimiterPos = strLine.find('=');
        if (nDelimiterPos == std::string::npos)
        {
            continue;
        }

        std::string strKey = strLine.substr(0, nDelimiterPos);
        std::string strValue = strLine.substr(nDelimiterPos + 1);

        if (strValue == "true" || strValue == "false")
        {
            m_mapBoolPreferences[strKey] = (strValue == "true");
        }
        else
        {
            try
            {
                int nIntValue = std::stoi(std::string(strValue));
                m_mapIntPreferences[strKey] = nIntValue;
            }
            catch (const std::exception&)
            {
                // Ignore invalid int values
            }
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
        writer << pair.first << "=" << (pair.second ? "true" : "false") << "\n";
    }

    for (const auto& pair : m_mapIntPreferences)
    {
        writer << pair.first << "=" << pair.second << "\n";
    }
}
