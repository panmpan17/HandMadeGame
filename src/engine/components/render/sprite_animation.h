#pragma once

#include "quad.h"
#include "../component.h"
#include "../../core/debug_macro.h"
#include "../../core/scene/node.h"
#include "../../../utils/expandable_array.h"
#include "../../../utils/platform.h"

#if IS_PLATFORM_MACOS
#define USHORT ushort
#elif IS_PLATFORM_WINDOWS
#include <windows.h>
#define USHORT USHORT
#endif

struct SpriteAnimationInfo
{
    SpriteAnimationInfo(USHORT nSpriteIndexArray[], int nArraySize, float fAnimateInterval)
        : m_nSpriteIndexArraySize(nArraySize), m_fAnimateInterval(fAnimateInterval)
    {
        m_nSpriteIndexArray = new USHORT[nArraySize];
        memcpy(m_nSpriteIndexArray, nSpriteIndexArray, sizeof(USHORT) * nArraySize);
    }

    ~SpriteAnimationInfo()
    {
        delete[] m_nSpriteIndexArray;
    }

    std::string m_strName;
    int m_nSpriteIndexArraySize;
    USHORT* m_nSpriteIndexArray;
    float m_fAnimateInterval;
};


class SpriteAnimation : public Component
{
public:
    SpriteAnimation() {}
    SpriteAnimation(Sprite* pSprite);

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    void openAnimationFile(const std::string_view& strFilePath);

    int addAnimationInfo(SpriteAnimationInfo* pAnimationInfo)
    {
        return m_pAnimationInfoArray.addElement(pAnimationInfo);
    }

    void playAnimationInfo(int nIndex)
    {
        if (nIndex < 0 || nIndex >= m_pAnimationInfoArray.getSize())
        {
            // LOGLN("Index wrong: {}", nIndex);
            return;
        }

        SpriteAnimationInfo* pInfo = m_pAnimationInfoArray.getElement(nIndex);
        if (pInfo == nullptr)
        {
            // LOGLN("No info: {}", nIndex);
            return;
        }

        // LOGLN("Playing animation: {}", nIndex);

        m_pAnimationInfo = pInfo;
        m_nActiveAnimationIndex = nIndex;
        m_nCurrentFrame = 0;
        m_fIntervalTimer = 0;

        m_pSprite->setSpriteIndex(m_pAnimationInfo->m_nSpriteIndexArray[m_nCurrentFrame]);
    }

    int getAnimationIndexByName(const std::string& name) const
    {
        for (int i = 0; i < m_pAnimationInfoArray.getSize(); ++i)
        {
            SpriteAnimationInfo* pInfo = m_pAnimationInfoArray.getElement(i);
            if (pInfo && pInfo->m_strName == name)
            {
                return i;
            }
        }
        return -1;
    }

    virtual void update(float deltaTime) override
    {
        if (!m_pAnimationInfo)
            return;

        Node* pNode = getNode();
        if (pNode)
        {
            m_fIntervalTimer += deltaTime;
            if (m_fIntervalTimer >= m_pAnimationInfo->m_fAnimateInterval)
            {
                m_fIntervalTimer = 0;

                m_nCurrentFrame = (m_nCurrentFrame + 1) % m_pAnimationInfo->m_nSpriteIndexArraySize;
                m_pSprite->setSpriteIndex(m_pAnimationInfo->m_nSpriteIndexArray[m_nCurrentFrame]);
            }
        }
    }

    virtual void draw() override {}

protected:
    Sprite* m_pSprite;
    std::string m_strAnimationFileName;
    PointerExpandableArray<SpriteAnimationInfo*> m_pAnimationInfoArray = PointerExpandableArray<SpriteAnimationInfo*>(5);
    SpriteAnimationInfo* m_pAnimationInfo = nullptr;
    int m_nActiveAnimationIndex = -1;
    int m_nCurrentFrame = 0;

    float m_fIntervalTimer = 0;

    COMPONENT_REGISTER_SERIALIZABLE(SpriteAnimation)
};

REGISTER_CLASS(SpriteAnimation)

