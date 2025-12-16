#pragma once

#include "../component.h"
#include "../../core/math/vector.h"
#include "../../core/scene/node.h"
#include "../../core/debug_macro.h"
// #include "../serialization/type_registry.h"


class TwoPointScaling : public Component
{
public:
    TwoPointScaling(const Vector3& start, const Vector3& end, float duration)
        : m_vecStart(start), m_vecEnd(end), m_fDuration(duration)
    {
    }

    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override
    {
        // Update the position based on the direction and speed
        auto pNode = getNode();
        if (!pNode) return;

        if (m_bIsMovingForward)
        {
            m_fTimer += fDeltaTime;
        }
        else
        {
            m_fTimer -= fDeltaTime;
        }

        if (m_fTimer >= m_fDuration) {
            m_fTimer = m_fDuration;
            m_bIsMovingForward = !m_bIsMovingForward;
        }
        if (m_fTimer <= 0) {
            m_fTimer = 0;
            m_bIsMovingForward = !m_bIsMovingForward;
        }

        pNode->setScale(Vector3::lerp(m_vecStart, m_vecEnd, m_fTimer / m_fDuration));
    }

    const Vector3& getStart() const { return m_vecStart; }
    void setStart(const Vector3& vecStart) { m_vecStart = vecStart; }

    const Vector3& getEnd() const { return m_vecEnd; }
    void setEnd(const Vector3& vecEnd) { m_vecEnd = vecEnd; }

    float getDuration() const { return m_fDuration; }
    void setDuration(float duration) { m_fDuration = duration; }

    virtual void onInspectorUI(int nComponentIndex) override;

protected:
    Vector3 m_vecStart;
    Vector3 m_vecEnd;
    float m_fDuration;

    bool m_bIsMovingForward = true;
    float m_fTimer = 0.0f;

    // COMPONENT_REGISTER_SERIALIZABLE(TwoPointScaling)
};

// REGISTER_CLASS(Rotate)