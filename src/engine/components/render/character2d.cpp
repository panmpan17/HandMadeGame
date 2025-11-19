#include "character2d.h"
#include "../../core/debug_macro.h"
#include "../../core/input/input_manager.h"
#include "../../core/serialization/serializer.h"

Character2d::Character2d(SpriteAnimation* pAnimation) : m_pAnimation(pAnimation)
{
    m_nIdleAnimationIndex = m_pAnimation->getAnimationIndexByName("idle");
    m_nWalkAnimationIndex = m_pAnimation->getAnimationIndexByName("walk");
    
    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_D, [this](bool bPressed) {
        if (bPressed) {
            m_pAnimation->playAnimationInfo(m_nWalkAnimationIndex);
        } else {
            m_pAnimation->playAnimationInfo(m_nIdleAnimationIndex);
        }
    });
}

void Character2d::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_pAnimation);
}

bool Character2d::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (Component::deserializeField(deserializer, strFieldName, strFieldValue))
    {
        return true;
    }

    IF_DESERIALIZE_FIELD_CHECK(m_pAnimation)
    {
        size_t nId = std::stoull(strFieldValue.data());
        deserializer.getSerializableFromId(nId, [this](ISerializable* pObj) {
            m_pAnimation = static_cast<SpriteAnimation*>(pObj);
        });
    }

    return false;
}

void Character2d::onNodeFinishedDeserialization()
{
    if (m_pAnimation)
    {
        m_nIdleAnimationIndex = m_pAnimation->getAnimationIndexByName("idle");
        m_nWalkAnimationIndex = m_pAnimation->getAnimationIndexByName("walk");
        
        InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_D, [this](bool bPressed) {
            if (bPressed) {
                m_pAnimation->playAnimationInfo(m_nWalkAnimationIndex);
            } else {
                m_pAnimation->playAnimationInfo(m_nIdleAnimationIndex);
            }
        });
    }
}

