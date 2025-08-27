#pragma once

#include "component.h"
#include "../node.h"
#include "../draw/quad.h"


struct SpriteAnimationInfo
{
    SpriteAnimationInfo(ushort nSpriteIndexArray[], int nArraySize, float fAnimateInterval)
        : m_fAnimateInterval(fAnimateInterval), m_nSpriteIndexArraySize(nArraySize)
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
    SpriteAnimation(Sprite* pSprite, SpriteAnimationInfo* pAnimationInfo) : m_pSprite(pSprite), m_pAnimationInfo(pAnimationInfo) {}

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float deltaTime) override
    {
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

private:
    Sprite* m_pSprite;
    SpriteAnimationInfo* m_pAnimationInfo = nullptr;
    int m_nCurrentFrame = 0;

    float m_fIntervalTimer = 0;
};

