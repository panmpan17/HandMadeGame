#include "particle_system.h"

#include <glad/gl.h>
#include "../../draw/shader.h"
#include "../../draw/vertex.h"
#include "../../draw/shader_loader.h"
#include "../../debug_macro.h"
#include "../../node.h"
#include "../../camera.h"
#include "../../window.h"
#include "../../random.h"
#include "../../image.h"
#include "../../platform.h"
#include "../../serializer.h"


#define SWAP_PARTICLE_POSITION(i, j) \
    do { \
        if (i == j) break; \
        std::swap(m_arrParticlesGPU[i], m_arrParticlesGPU[j]); \
        std::swap(m_arrParticlesCPU[i], m_arrParticlesCPU[j]); \
    } while (0)


ParticleSystem::ParticleSystem(int nParticleCount, bool bSimulateInLocal/* = false */)
{
    m_nAllParticleCount = nParticleCount;
    m_arrParticlesGPU = new ParticleGPUInstance[nParticleCount];
    m_arrParticlesCPU = new ParticleCPUInstance[nParticleCount];

    m_nAliveParticleCount = 0;
    m_nLastAliveParticleIndex = -1;

    for (int i = 0; i < nParticleCount; ++i)
    {
        m_arrParticlesCPU[i].m_fLifetime = 0;
    }

    for (int i = 0; i < 4; ++i)
    {
        m_arrParticleModules[i] = nullptr;
    }

    for (int i = 0; i < 4; ++i)
    {
        m_arrParticleIndividualModules[i] = nullptr;
    }

    m_bSimulateInLocal = bSimulateInLocal;
}

ParticleSystem::~ParticleSystem()
{
    delete[] m_arrParticlesGPU;
    delete[] m_arrParticlesCPU;

    for (IParticleModule*& pModule : m_arrParticleModules)
    {
        if (pModule)
        {
            delete pModule;
        }
    }

    for (IParticleIndividualModule*& pModule : m_arrParticleIndividualModules)
    {
        if (pModule)
        {
            delete pModule;
        }
    }

    glDeleteBuffers(1, &m_nInstanceBuffer);
    glDeleteVertexArrays(1, &m_nVertexArray);
    glDeleteBuffers(1, &m_nVertexBuffer);
}

void ParticleSystem::registerBuffer()
{
    // Four corner vertex datas
    VertexWUV arrQuadVerticies[4];
    arrQuadVerticies[0] = { { -0.1f, -0.1f }, { 0.f, 0.f } };
    arrQuadVerticies[1] = { { 0.1f, -0.1f }, { 1.f, 0.f } };
    arrQuadVerticies[2] = { { -0.1f, 0.1f }, { 0.f, 1.f } };
    arrQuadVerticies[3] = { { 0.1f, 0.1f }, { 1.f, 1.f } };

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrQuadVerticies), arrQuadVerticies, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));


    // Instance data
    glGenBuffers(1, &m_nInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nInstanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_nAllParticleCount * sizeof(ParticleGPUInstance), m_arrParticlesGPU, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_vecPosition));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_vecColor));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_fRotation));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_fScale));
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::setShader(Shader* pShader)
{
    m_pShader = pShader;

    m_nMVPUniForm = pShader->getUniformLocation("u_MVP");
    m_nNodeTransformUniform = pShader->getUniformLocation("u_nodeTransform");
    m_nUseNodeTransformUniform = pShader->getUniformLocation("u_useNodeTransform");
    m_nUseTextureUniform = pShader->getUniformLocation("u_useTexture");
    m_nTextureUniform = pShader->getUniformLocation("u_tex0");
}

void ParticleSystem::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    if (m_nAliveParticleCount <= 0) return;

    glBindVertexArray(m_nVertexArray);
    glUseProgram(m_pShader->getProgram());

    glUniform1i(m_nUseNodeTransformUniform, m_bSimulateInLocal ? 1 : 0);
    glUniform1i(m_nUseTextureUniform, m_pImage ? 1 : 0);

    glUniform1i(m_nTextureUniform, 0);

    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();

    glUniformMatrix4fv(m_nMVPUniForm, 1, GL_FALSE, (const GLfloat*) cameraViewMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pImage ? m_pImage->getTextureID() : 0);

    if (m_bSimulateInLocal)
    {
        mat4x4 nodeTransform;

        mat4x4_identity(nodeTransform);
        const vec3& nodePosition = getNode()->getPosition();
        mat4x4_translate(nodeTransform, nodePosition[0], nodePosition[1], nodePosition[2]);

        glUniformMatrix4fv(m_nNodeTransformUniform, 1, GL_FALSE, (const GLfloat*) nodeTransform);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_nInstanceBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_nAliveParticleCount * sizeof(ParticleGPUInstance), m_arrParticlesGPU);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_nAliveParticleCount);
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void ParticleSystem::update(float fDeltaTime)
{
    m_fActiveTimer += fDeltaTime;
    if (m_fActiveTimer >= m_fActiveTime)
    {
        if (m_bIsLooping)
        {
            m_fActiveTimer = 0.0f;
            for (IParticleModule*& pModule : m_arrParticleModules)
            {
                if (pModule)
                {
                    pModule->onActiveTimeReset();
                }
            }
        }
        else
        {
            m_bIsEmitting = false;
        }
    }

    for (IParticleModule*& pModule : m_arrParticleModules)
    {
        if (pModule)
        {
            pModule->update(*this, fDeltaTime);
        }
    }

    for (int i = 0; i < m_nAllParticleCount; ++i)
    {
        updateParticle(i, fDeltaTime);
    }
}

void ParticleSystem::updateParticle(int& nIndex, float fDeltaTime)
{
    if (!m_arrParticlesCPU[nIndex].isAlive()) return;

    m_arrParticlesCPU[nIndex].m_fLifetime -= fDeltaTime;

    if (!m_arrParticlesCPU[nIndex].isAlive())
    {
        SWAP_PARTICLE_POSITION(nIndex, m_nLastAliveParticleIndex);
        --m_nLastAliveParticleIndex;
        --m_nAliveParticleCount;
        --nIndex;
        return;
    }

    m_arrParticlesGPU[nIndex].m_fRotation += fDeltaTime * m_arrParticlesCPU[nIndex].m_fRotationSpeed;

    m_arrParticlesCPU[nIndex].m_vecVelocity[0] += m_fGravity[0] * fDeltaTime;
    m_arrParticlesCPU[nIndex].m_vecVelocity[1] += m_fGravity[1] * fDeltaTime;

    m_arrParticlesGPU[nIndex].m_vecPosition[0] += m_arrParticlesCPU[nIndex].m_vecVelocity[0] * fDeltaTime;
    m_arrParticlesGPU[nIndex].m_vecPosition[1] += m_arrParticlesCPU[nIndex].m_vecVelocity[1] * fDeltaTime;

    for (IParticleIndividualModule*& pModule : m_arrParticleIndividualModules)
    {
        if (pModule)
        {
            pModule->update(*this, &m_arrParticlesGPU[nIndex], &m_arrParticlesCPU[nIndex], fDeltaTime);
        }
    }
}

void ParticleSystem::spawnNewParticles(int nSpawnCount/* = 1*/)
{
    if (!m_bIsEmitting)
    {
        return;
    }

    for (int i = m_nLastAliveParticleIndex + 1, spawned = 0; i < m_nAllParticleCount && spawned < nSpawnCount; ++i, ++spawned)
    {
        if (!m_arrParticlesCPU[i].isAlive())
        {
            m_arrParticlesCPU[i].m_fLifetime = randomFloat(m_fLifetimeMin, m_fLifetimeMax); // Random lifetime
            m_arrParticlesCPU[i].m_fMaxLifetime = m_arrParticlesCPU[i].m_fLifetime;
            m_arrParticlesCPU[i].m_fRotationSpeed = randomFloat(m_fStartRotationSpeedMin, m_fStartRotationSpeedMax); // Random m_fRotation speed

            float vecBasePositionX, vecBasePositionY;
            if (m_bSimulateInLocal)
            {
                vecBasePositionX = 0;
                vecBasePositionY = 0;
            }
            else
            {
                const vec3& vecBasePosition = getNode()->getPosition();
                vecBasePositionX = vecBasePosition[0];
                vecBasePositionY = vecBasePosition[1];
            }

            switch (m_eSpawnShape)
            {
                case eParticleSpawnShape::DOT:
                    m_arrParticlesGPU[i].m_vecPosition[0] = vecBasePositionX;
                    m_arrParticlesGPU[i].m_vecPosition[1] = vecBasePositionY;
                    break;

                case eParticleSpawnShape::CIRCLE:
                    {
                        float fAngle = randomFloat(0.0f, 2.0f * M_PI);
                        float fRadius = randomFloat(0.0f, m_fSpawnShapeWidth);
                        m_arrParticlesGPU[i].m_vecPosition[0] = cos(fAngle) * fRadius + vecBasePositionX;
                        m_arrParticlesGPU[i].m_vecPosition[1] = sin(fAngle) * fRadius + vecBasePositionY;
                    }
                    break;

                case eParticleSpawnShape::BOX:
                    m_arrParticlesGPU[i].m_vecPosition[0] = randomFloat(-m_fSpawnShapeWidth, m_fSpawnShapeWidth) + vecBasePositionX;
                    m_arrParticlesGPU[i].m_vecPosition[1] = randomFloat(-m_fSpawnShapeHeight, m_fSpawnShapeHeight) + vecBasePositionY;
                    break;
            }

            float fStartVelocity = randomFloat(m_fStartVelocityMin, m_fStartVelocityMax);
            if (m_funcStartVelocityDirectionOverride)
            {
                m_funcStartVelocityDirectionOverride(m_arrParticlesCPU[i].m_vecVelocity);
            }
            else
            {
                randomOnUnitCircle(m_arrParticlesCPU[i].m_vecVelocity);
            }

            m_arrParticlesCPU[i].m_vecVelocity[0] *= fStartVelocity;
            m_arrParticlesCPU[i].m_vecVelocity[1] *= fStartVelocity;
            // m_arrParticlesGPU[i]

            randomBetweenVec4(m_arrParticlesGPU[i].m_vecColor, m_vecStartColorMin, m_vecStartColorMax);
            m_arrParticlesGPU[i].m_fRotation = randomFloat(m_fStartRotationMin, m_fStartRotationMax);
            m_arrParticlesGPU[i].m_fScale = randomFloat(m_fStartScaleMin, m_fStartScaleMax);
            m_arrParticlesCPU[i].m_fBaseScale = m_arrParticlesGPU[i].m_fScale;

            ++m_nAliveParticleCount;
            ++m_nLastAliveParticleIndex;
        }
    }
}

void ParticleSystem::sortAliveParticleInFront()
{
    for (int i = 0; i < m_nAllParticleCount; i++)
    {
        for (int e = i + 1; e < m_nAllParticleCount; e++)
        {
            if (!m_arrParticlesCPU[i].isAlive() && m_arrParticlesCPU[e].isAlive())
            {
                SWAP_PARTICLE_POSITION(i, e);
            }
        }
    }

    for (int i = 0; i < m_nAllParticleCount; i++)
    {
        if (m_arrParticlesCPU[i].isAlive())
        {
            m_nLastAliveParticleIndex = i;
        }
        else
        {
            break;
        }
    }
}

void ParticleSystem::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_nAllParticleCount);
    serializer.ADD_ATTRIBUTES_VALUE(m_eSpawnShape, static_cast<int>(m_eSpawnShape));
    serializer.ADD_ATTRIBUTES(m_fSpawnShapeWidth);
    serializer.ADD_ATTRIBUTES(m_fSpawnShapeHeight);
    serializer.ADD_ATTRIBUTES(m_fLifetimeMin);
    serializer.ADD_ATTRIBUTES(m_fLifetimeMax);
    serializer.ADD_ATTRIBUTES(m_fStartRotationMin);
    serializer.ADD_ATTRIBUTES(m_fStartRotationMax);
    serializer.ADD_ATTRIBUTES(m_fStartRotationSpeedMin);
    serializer.ADD_ATTRIBUTES(m_fStartRotationSpeedMax);
    serializer.ADD_ATTRIBUTES(m_fStartScaleMin);
    serializer.ADD_ATTRIBUTES(m_fStartScaleMax);
    serializer.ADD_ATTRIBUTES(m_vecStartColorMin);
    serializer.ADD_ATTRIBUTES(m_vecStartColorMax);
    serializer.ADD_ATTRIBUTES(m_fStartVelocityMin);
    serializer.ADD_ATTRIBUTES(m_fStartVelocityMax);

    if (m_pShader)
    {
        serializer.ADD_ATTRIBUTES_VALUE(m_pShader, m_pShader->getId());
    }
}

void ParticleSystem::deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    DESERIALIZE_FIELD(m_nAllParticleCount);

    IF_DESERIALIZE_FIELD_CHECK(m_eSpawnShape)
    {
        m_eSpawnShape = static_cast<eParticleSpawnShape>(std::atoi(strFieldValue.data()));
    }

    DESERIALIZE_FIELD(m_fSpawnShapeWidth);
    DESERIALIZE_FIELD(m_fSpawnShapeHeight);
    DESERIALIZE_FIELD(m_fLifetimeMin);
    DESERIALIZE_FIELD(m_fLifetimeMax);
    DESERIALIZE_FIELD(m_fStartRotationMin);
    DESERIALIZE_FIELD(m_fStartRotationMax);
    DESERIALIZE_FIELD(m_fStartRotationSpeedMin);
    DESERIALIZE_FIELD(m_fStartRotationSpeedMax);
    DESERIALIZE_FIELD(m_fStartScaleMin);
    DESERIALIZE_FIELD(m_fStartScaleMax);
    DESERIALIZE_FIELD(m_vecStartColorMin);
    DESERIALIZE_FIELD(m_vecStartColorMax);
    DESERIALIZE_FIELD(m_fStartVelocityMin);
    DESERIALIZE_FIELD(m_fStartVelocityMax);

    IF_DESERIALIZE_FIELD_CHECK(m_pShader)
    {
        m_pShader = ShaderLoader::getInstance()->getShader(std::atoi(strFieldValue.data()));
    }
}

void ParticleSystem::onNodeFinishedDeserialization()
{
    if (m_pShader)
    {
        setShader(m_pShader);
        registerBuffer();
    }
}
