#include "serializer.h"
#include "node.h"
#include "iserializable.h"
#include "draw/triangle.h"

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
            m_bIsClassStarted = true;

            if (memcmp(m_strClassName.data(), "Node", 4) == 0)
            {
                m_pCurrentDeserializingObject = new Node();
            }
            if (memcmp(m_strClassName.data(), "8Triangle", 8) == 0)
            {
                m_pCurrentDeserializingObject = new Triangle();
            }

            std::cout << "Class " << m_strClassName <<  " start" << "\n";
        }
        else if (memcmp(&strBack, "}", 1) == 0) {
            if (m_bIsClassStarted) {

                // TODO: store it somewhere
                // std::cout << "Class " << m_strClassName << ' ' << *(static_cast<Node*>(m_pCurrentDeserializingObject)) <<  " end" << "\n";

                m_vecDeserializedObjects.push_back(m_pCurrentDeserializingObject);
                m_pCurrentDeserializingObject = nullptr;

                m_bIsClassStarted = false;                
            }
        }
        else if (m_bIsClassStarted) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::cout << "Deserializing field " << line.substr(0, pos) << " with value " << line.substr(pos + 2) << "\n";
                m_pCurrentDeserializingObject->deserializeField(line.substr(0, pos), line.substr(pos + 2));
            }
        }
    }
}