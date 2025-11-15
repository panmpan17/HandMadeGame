#include "time.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "debug_macro.h"


TimeManager::TimeManager()
{
    glGenBuffers(1, &m_nTimeUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_nTimeUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float) + 4, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TimeManager::onWindowStart()
{
    m_fStartTime = m_fCurrentDrawTime = glfwGetTime();
    m_fLastDrawTime = m_fCurrentDrawTime;
    m_fDeltaTime = 0.0f;
}

float TimeManager::preUpdate()
{
    m_fCurrentDrawTime = glfwGetTime();
    m_fDeltaTime = m_fCurrentDrawTime - m_fLastDrawTime;
    m_fLastDrawTime = m_fCurrentDrawTime;
    m_fTotalTime += m_fDeltaTime;
    // LOG("Delta Time: {}, Total Time: {}\r", m_fDeltaTime, m_fTotalTime);

    glBindBuffer(GL_UNIFORM_BUFFER, m_nTimeUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &m_fTotalTime);
    // glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float), sizeof(float), &m_fDeltaTime);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return m_fDeltaTime;
}