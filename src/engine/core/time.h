#pragma once


typedef unsigned int GLuint;


class TimeManager
{
public:
    static TimeManager* getInstance() { return m_sInstance; }

    static void Initialize()
    {
        if (!m_sInstance)
        {
            m_sInstance = new TimeManager();
        }
    }

    float getDeltaTime() const { return m_fDeltaTime; }

    void onWindowStart();
    float preUpdate();

    inline GLuint getTimeUBO() const { return m_nTimeUBO; }

protected:
    static inline TimeManager* m_sInstance = nullptr;

    TimeManager();
    
    double m_fStartTime = 0.0;
    double m_fCurrentDrawTime = 0.0;
    double m_fLastDrawTime = 0.0;
    float m_fDeltaTime = 0.0;
    float m_fTotalTime = 0.0f;

    GLuint m_nTimeUBO = 0;
};