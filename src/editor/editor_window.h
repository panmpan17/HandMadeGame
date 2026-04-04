#pragma once


class IEditorWindow
{
public:
    IEditorWindow() = default;
    ~IEditorWindow() = default;

    virtual void update() = 0;

    inline bool isActive() const { return m_bIsActive; }
    inline void setActive(bool bActive) { m_bIsActive = bActive; }

protected:
    bool m_bIsActive = true;
};
