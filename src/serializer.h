#pragma once


#include <linmath.h>
#include <fstream>
#include <iostream>
#include <vector>


#define CHECK_FILE_IS_OPEN if (!m_oOutputFile.is_open()) { std::cerr << "Error: File not open\n"; return; }
#define ADD_ATTRIBUTES(fieldName) addAttributes(#fieldName, fieldName)
#define ADD_ATTRIBUTES_VALUE(fieldName, value) addAttributes(#fieldName, value)
#define DESERIALIZE_FIELD(fileName) \
    if (memcmp(strFieldName.data(), #fileName, sizeof(#fileName) - 1) == 0) { \
        DataDeserializer::deserializeField(fileName, strFieldValue); \
        return true; \
    }
#define IF_DESERIALIZE_FIELD_CHECK(fileName) \
    if (memcmp(strFieldName.data(), #fileName, sizeof(#fileName) - 1) == 0)

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
    void addAttributes(const std::string_view& strAttributeNames, const vec4& vecValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << vecValue[0] << ", " << vecValue[1] << ", " << vecValue[2] << ", " << vecValue[3] << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, bool bValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << bValue << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, const std::string_view& strValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << strValue << "\n";
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
    static void deserializeField(vec4& outVec, const std::string_view& strFieldValue)
    {
        sscanf(strFieldValue.data(), "%f, %f, %f, %f", &outVec[0], &outVec[1], &outVec[2], &outVec[3]);
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

    DataDeserializer(const std::string_view& strFilename)
    {
        m_oInputFile.open(strFilename, std::ios::in);
    }

    ~DataDeserializer()
    {
        if (m_oInputFile.is_open()) {
            m_oInputFile.close();
        }
    }


    const std::vector<ISerializable*>& getDeserializedObjects() const { return m_vecDeserializedObjects; }

    void read();

protected:
    std::ifstream m_oInputFile;
    bool m_bIsClassStarted = false;
    std::string_view m_strClassName;

    ISerializable* m_pCurrentDeserializingObject = nullptr;
    std::vector<ISerializable*> m_vecDeserializedObjects;
};