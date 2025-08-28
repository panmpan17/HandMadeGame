#pragma once


#include <linmath.h>
#include <fstream>
#include <iostream>


#define CHECK_FILE_IS_OPEN if (!m_oOutputFile.is_open()) { std::cerr << "Error: File not open\n"; return; }
#define ADD_ATTRIBUTES(fieldName) addAttributes(#fieldName, fieldName)
#define DESERIALIZE_FIELD(fileName) \
    if (memcmp(strFieldName.data(), #fileName, sizeof(#fileName) - 1) == 0) { \
        DataDeserializer::deserializeField(fileName, strFieldValue); \
        return; \
    }

class ISerializable;

class DataSerializer
{
public:
    DataSerializer(const std::string& filename)
    {
        // m_oOutputFile.open(filename, std::ios::binary);
        m_oOutputFile.open(filename, std::ios::out);
        // if (!m_oOutputFile.is_open()) {
        // }
    }

    ~DataSerializer()
    {
        if (m_oOutputFile.is_open()) {
            m_oOutputFile.close();
        }
    }

    void startClassHeader(const std::string_view& strClassName)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strClassName << " {\n";
    }
    void endClassHeader()
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << "}\n";
    }

    void addAttributes(const std::string_view& strAttributeNames, int nValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << nValue << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, float fValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << fValue << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, const vec2& vecValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << vecValue[0] << ", " << vecValue[1] << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, const vec3& vecValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << vecValue[0] << ", " << vecValue[1] << ", " << vecValue[2] << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, bool bValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << bValue << "\n";
    }

    DataSerializer& operator<<(const ISerializable* pObject);

    void finish()
    {
        if (m_oOutputFile.is_open()) {
            m_oOutputFile.close();
        }
    }

private:
    std::ofstream m_oOutputFile; 
};

class DataDeserializer
{
public:
    static void deserializeField(vec2& outVec, const std::string_view& strFieldValue)
    {
        sscanf(strFieldValue.data(), "%f, %f", &outVec[0], &outVec[1]);
    }
    static void deserializeField(vec3& outVec, const std::string_view& strFieldValue)
    {
        sscanf(strFieldValue.data(), "%f, %f, %f", &outVec[0], &outVec[1], &outVec[2]);
    }
    static void deserializeField(int& outInt, const std::string_view& strFieldValue)
    {
        outInt = std::stoi(strFieldValue.data());
    }
    static void deserializeField(float& outFloat, const std::string_view& strFieldValue)
    {
        outFloat = std::stof(strFieldValue.data());
    }
    static void deserializeField(bool& outBool, const std::string_view& strFieldValue)
    {
        outBool = std::string(strFieldValue) == "1";
    }

    DataDeserializer(const std::string& filename)
    {
        m_oInputFile.open(filename, std::ios::in);
    }

    void read();

private:
    std::ifstream m_oInputFile;
    bool m_bIsClassStarted = false;
    std::string_view m_strClassName;
    // std::vector<std::pair<std::string, std::string>> m_vecFieldInfo;

    ISerializable* m_pCurrentDeserializingObject = nullptr;
};