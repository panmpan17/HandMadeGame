#pragma once

#include "component.h"
#include "../node.h"
#include "../expandable_array.h"
#include "../draw/quad.h"
#include "../debug_macro.h"


struct SpriteAnimationInfo
{
    SpriteAnimationInfo(ushort nSpriteIndexArray[], int nArraySize, float fAnimateInterval)
        : m_nSpriteIndexArraySize(nArraySize), m_fAnimateInterval(fAnimateInterval)
    {
        m_nSpriteIndexArray = new ushort[nArraySize];
        memcpy(m_nSpriteIndexArray, nSpriteIndexArray, sizeof(ushort) * nArraySize);
    }

    ~SpriteAnimationInfo()
    {
        delete[] m_nSpriteIndexArray;
    }

    int m_nSpriteIndexArraySize;
    ushort* m_nSpriteIndexArray;
    float m_fAnimateInterval;
};


class SpriteAnimation : public Component
{
public:
    SpriteAnimation(Sprite* pSprite) : m_pSprite(pSprite) {}

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    int addAnimationInfo(SpriteAnimationInfo* pAnimationInfo)
    {
        return m_pAnimationInfoArray.addElement(pAnimationInfo);
    }

    void playAnimationInfo(int nIndex)
    {
        if (nIndex < 0 || nIndex >= m_pAnimationInfoArray.getSize())
        {
            // LOGLN_EX("Index wrong: {}", nIndex);
            return;
        }

        SpriteAnimationInfo* pInfo = m_pAnimationInfoArray.getElement(nIndex);
        if (pInfo == nullptr)
        {
            // LOGLN_EX("No info: {}", nIndex);
            return;
        }

        // LOGLN_EX("Playing animation: {}", nIndex);

        m_pAnimationInfo = pInfo;
        m_nActiveAnimationIndex = nIndex;
        m_nCurrentFrame = 0;
        m_fIntervalTimer = 0;

        m_pSprite->setSpriteIndex(m_pAnimationInfo->m_nSpriteIndexArray[m_nCurrentFrame]);
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
    PointerExpandableArray<SpriteAnimationInfo*> m_pAnimationInfoArray = PointerExpandableArray<SpriteAnimationInfo*>(5);
    SpriteAnimationInfo* m_pAnimationInfo = nullptr;
    int m_nActiveAnimationIndex = -1;
    int m_nCurrentFrame = 0;

    float m_fIntervalTimer = 0;

    virtual void serializeToWrapper(DataSerializer& serializer) const override;
};

