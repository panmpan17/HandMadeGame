#include "sprite_animation.h"
#include "../serializer.h"
#include "../file_utils.h"
#include "../random.h"


SpriteAnimation::SpriteAnimation(Sprite* pSprite) : m_pSprite(pSprite)
{
    m_nID = generateRandomUUID();
}

void SpriteAnimation::openAnimationFile(const std::string_view& strFilePath)
{
    FileReader fileReader(strFilePath);
    if (!fileReader.isOpen())
    {
        // Handle error
        return;
    }

    // int nCurrentShaderId = -1;
    std::string strCurrentAnimationName;//, strCurrentVertexPath, strCurrentFragmentPath;
    float fCurrentAnimationInterval = 0.0f;

    // Read shader paths from the registry file
    std::string strLine;
    while (fileReader.readLine(strLine))
    {
        if (strLine.front() == '#')
        {
            continue;
        }

        if (strLine.front() != ' ')
        {
            strCurrentAnimationName = strLine.substr(0, strLine.length() - 1);
        }
        else if (memcmp(strLine.data() + 2, "interval", 8) == 0)
        {
            fCurrentAnimationInterval = std::atof(strLine.substr(2 + 10).c_str());
        }
        else if (memcmp(strLine.data() + 2, "frames", 6) == 0)
        {
            std::string strFrames = strLine.substr(2 + 8);
            std::vector<ushort> vecFrames;
            size_t start = 0;
            size_t end = strFrames.find(',');
            while (end != std::string::npos)
            {
                vecFrames.push_back(static_cast<ushort>(std::stoi(strFrames.substr(start, end - start))));
                start = end + 1;
                end = strFrames.find(',', start);
            }
            vecFrames.push_back(static_cast<ushort>(std::stoi(strFrames.substr(start))));

            if (!strCurrentAnimationName.empty() && fCurrentAnimationInterval > 0.0f && !vecFrames.empty())
            {
                auto pInfo = new SpriteAnimationInfo(vecFrames.data(), vecFrames.size(), fCurrentAnimationInterval);
                pInfo->m_strName = strCurrentAnimationName;
                int nIndex = addAnimationInfo(pInfo);
                if (nIndex == 0)
                {
                    playAnimationInfo(nIndex);
                }
            }

            strCurrentAnimationName = "";
            fCurrentAnimationInterval = 0.0f;
        }
    }

    if (!strCurrentAnimationName.empty())
    {
        playAnimationInfo(0);
    }

    m_strAnimationFileName = strFilePath.data();
}


void SpriteAnimation::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_pSprite);
    serializer.ADD_ATTRIBUTES(m_strAnimationFileName);

    // TODO: add m_pSprite link
    // TODO: add animation infos
}

bool SpriteAnimation::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (Component::deserializeField(deserializer, strFieldName, strFieldValue))
    {
        return true;
    }

    IF_DESERIALIZE_FIELD_CHECK(m_pSprite)
    {
        size_t nId = std::stoull(strFieldValue.data());
        deserializer.getSerializableFromId(nId, [this](ISerializable* pObj) {
            m_pSprite = static_cast<Sprite*>(pObj);
        });
    }

    IF_DESERIALIZE_FIELD_CHECK(m_strAnimationFileName)
    {
        openAnimationFile(strFieldValue);
    }

    return false;
}

void SpriteAnimation::onNodeFinishedDeserialization()
{
    
}
