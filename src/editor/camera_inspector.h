#pragma once


class CameraInspector
{
public:
    CameraInspector();
    ~CameraInspector();

    void update(float fDeltaTime);

private:
    bool m_bIsActive = true;
    float m_vecColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
};
