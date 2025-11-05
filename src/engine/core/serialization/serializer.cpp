#include "serializer.h"
#include "iserializable.h"
#include "../debug_macro.h"
#include "../scene/node.h"

void DataSerializer::addAttributes(const std::string_view& strAttributeNames, ISerializable* pValue)
{
    CHECK_FILE_IS_OPEN;
    m_oOutputFile << strAttributeNames << ": " << (pValue ? pValue->getID() : 0) << "\n";
}

DataSerializer& DataSerializer::operator<<(const ISerializable* pObject)
{
    if (pObject)
    {
        pObject->serializedTo(*this);
    }
    return *this;
}


void DataDeserializer::read()
{
    if (!m_oInputFile.is_open()) {
        std::cerr << "Error: File not open\n";
        return;
    }

    std::string line;
    while (std::getline(m_oInputFile, line)) {
        const char strBack = line.back();
        if (memcmp(&strBack, "{", 1) == 0) {
            m_strClassName = line.substr(0, line.size() - 2);

            const char* strClassName = m_strClassName.data();
            // std::cout << "Class " << m_strClassName <<  " start" << "\n";

            if (memcmp(strClassName, "Node", 4) == 0)
            {
                m_pCurrentDeserializingObject = new Node();
            }
            else
            {
                m_pCurrentDeserializingObject = TypeRegistry::instance().create(std::string(m_strClassName));
                if (!m_pCurrentDeserializingObject)
                {
                    LOGERR("Failed to create object of type: {}", m_strClassName);
                }
            }

            m_bIsClassStarted = m_pCurrentDeserializingObject != nullptr;
        }
        else if (memcmp(&strBack, "}", 1) == 0)
        {
            if (m_bIsClassStarted)
            {
                m_vecDeserializedObjects.push_back(m_pCurrentDeserializingObject);


                size_t nId = m_pCurrentDeserializingObject->getID();
                // LOGLN("Deserialized object of type: {}, ID {}", typeid(*m_pCurrentDeserializingObject).name(), nId);
                if (nId != 0)
                {
                    m_mapIDToObject[nId] = m_pCurrentDeserializingObject;

                    auto iterPending = m_vecPendingCallbacks.find(nId);
                    if (iterPending != m_vecPendingCallbacks.end())
                    {
                        for (const auto& callback : iterPending->second)
                        {
                            callback(m_mapIDToObject[nId]);
                        }
                        m_vecPendingCallbacks.erase(iterPending);
                    }
                }

                m_pCurrentDeserializingObject = nullptr;

                m_bIsClassStarted = false;                
            }
        }
        else if (m_bIsClassStarted && m_pCurrentDeserializingObject)
        {
            size_t pos = line.find(":");
            if (pos != std::string::npos)
            {
                // std::cout << "Deserializing field " << line.substr(0, pos) << " with value " << line.substr(pos + 2) << "\n";
                m_pCurrentDeserializingObject->deserializeField(*this, line.substr(0, pos), line.substr(pos + 2));
            }
        }
    }
}

void DataDeserializer::getSerializableFromId(size_t nId, std::function<void(ISerializable*)> outSerializable)
{
    auto iterSerializable = m_mapIDToObject.find(nId);
    if (iterSerializable != m_mapIDToObject.end())
    {
        // LOGLN("Deserialized object of type: {}", nId);
        outSerializable(iterSerializable->second);
        return;
    }

    auto iterPending = m_vecPendingCallbacks.find(nId);
    if (iterPending != m_vecPendingCallbacks.end())
    {
        // LOGLN("Deserialized object of type: {}, add to pending callbacks", nId);
        iterPending->second.push_back(outSerializable);
    }
    else
    {
        // LOGLN("Deserialized object of type: {}, add to pending callbacks, new list", nId);
        m_vecPendingCallbacks[nId] = { outSerializable };
    }
}