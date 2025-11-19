#pragma once


#include <linmath.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
#include "../math/vector.h"
#include "../../render/shader.h"
#include "../../render/shader_loader.h"
#include "../../render/image_loader.h"
#include "../../../utils/platform.h"


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
    void addAttributes(const std::string_view& strAttributeNames, size_t nValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << nValue << "\n";
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
    void addAttributes(const std::string_view& strAttributeNames, const Vector2& vecValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << vecValue.x << ", " << vecValue.y << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, const Vector3& vecValue)
    {
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << vecValue.x << ", " << vecValue.y << ", " << vecValue.z << "\n";
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
    void addAttributes(const std::string_view& strAttributeNames, const Shader* pShader)
    {
        if (!pShader) return;
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << pShader->getId() << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, const Image* pImage)
    {
        if (!pImage) return;
        CHECK_FILE_IS_OPEN;
        m_oOutputFile << strAttributeNames << ": " << pImage->getPath() << "\n";
    }
    void addAttributes(const std::string_view& strAttributeNames, ISerializable* pValue);

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
    static void deserializeField(Vector2& outVec, const std::string_view& strFieldValue)
    {
        sscanf(strFieldValue.data(), "%f, %f", &outVec.x, &outVec.y);
    }
    static void deserializeField(Vector3& outVec, const std::string_view& strFieldValue)
    {
        sscanf(strFieldValue.data(), "%f, %f, %f", &outVec.x, &outVec.y, &outVec.z);
    }
    static void deserializeField(int& outInt, const std::string_view& strFieldValue)
    {
        outInt = std::stoi(strFieldValue.data());
    }
    static void deserializeField(float& outFloat, const std::string_view& strFieldValue)
    {
        outFloat = std::stof(strFieldValue.data());
    }
    static void deserializeField(size_t& outSizeT, const std::string_view& strFieldValue)
    {
        outSizeT = static_cast<size_t>(std::stoull(strFieldValue.data()));
    }
    static void deserializeField(bool& outBool, const std::string_view& strFieldValue)
    {
        outBool = std::string(strFieldValue) == "1";
    }
    static void deserializeField(Shader*& pShader, const std::string_view& strFieldValue)
    {
        pShader = ShaderLoader::getInstance()->getShader(std::atoi(strFieldValue.data()));
    }
    static void deserializeField(Image*& pImage, const std::string_view& strFieldValue)
    {
        pImage = ImageLoader::getInstance()->getImageByPath(strFieldValue);
    }

    DataDeserializer(const std::string_view& strFilename)
    {
#if IS_PLATFORM_MACOS
        m_oInputFile.open(strFilename, std::ios::in);
#elif IS_PLATFORM_WINDOWS
        m_oInputFile.open(strFilename.data(), std::ios::in);
#endif
    }

    ~DataDeserializer()
    {
        if (m_oInputFile.is_open()) {
            m_oInputFile.close();
        }
    }


    const std::vector<ISerializable*>& getDeserializedObjects() const { return m_vecDeserializedObjects; }

    void read();

    void getSerializableFromId(size_t nId, std::function<void(ISerializable*)> outSerializable);

protected:
    std::ifstream m_oInputFile;
    bool m_bIsClassStarted = false;
    std::string_view m_strClassName;

    ISerializable* m_pCurrentDeserializingObject = nullptr;
    std::vector<ISerializable*> m_vecDeserializedObjects;
    // Don't create or delete anything when using this map
    std::unordered_map<size_t, ISerializable*> m_mapIDToObject;
    std::unordered_map<size_t, std::vector<std::function<void(ISerializable*)>>> m_vecPendingCallbacks;
};