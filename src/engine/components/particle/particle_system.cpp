#include "particle_system.h"

#include <glad/gl.h>
#include "../../render/shader.h"
#include "../../render/vertex.h"
#include "../../render/shader_loader.h"
#include "../../render/image.h"
#include "../../render/material.h"
#include "../../render/core/renderer.h"
#include "../../core/debug_macro.h"
#include "../../core/scene/node.h"
#include "../../core/camera.h"
#include "../../core/window.h"
#include "../../core/math/random.h"
#include "../../core/serialization/serializer.h"
#include "../../../utils/platform.h"
#include "../../../editor/gizmos.h"


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

    if (Window::ins->isUsingOpenGL())
    {
        glDeleteBuffers(1, &m_nInstanceBuffer);
        glDeleteVertexArrays(1, &m_nVertexArray);
        glDeleteBuffers(1, &m_nVertexBuffer);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        m_pVertexBuffer->release();
        m_pInstanceBuffer->release();
    }
#endif // __APPLE__
}

void ParticleSystem::registerBuffer()
{
    VertexWUV arrQuadVerticies[4];
    arrQuadVerticies[0] = { { -.5f, -.5f }, { 0.f, 0.f } };
    arrQuadVerticies[1] = { { .5f, -.5f }, { 1.f, 0.f } };
    arrQuadVerticies[2] = { { -.5f, .5f }, { 0.f, 1.f } };
    arrQuadVerticies[3] = { { .5f, .5f }, { 1.f, 1.f } };

    if (Window::ins->isUsingOpenGL())
    {
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
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_vecPosition));
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

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleGPUInstance), (void*)offsetof(ParticleGPUInstance, m_fOpacity));
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        MTL::Device* pDevice = Window::ins->getMetalDevice();
        m_pVertexBuffer = pDevice->newBuffer(arrQuadVerticies, sizeof(arrQuadVerticies), MTL::ResourceStorageModeShared);

        m_pInstanceBuffer = pDevice->newBuffer(m_nAllParticleCount * sizeof(ParticleGPUInstance), MTL::ResourceStorageModeShared);
    }
#endif // __APPLE__
}

void ParticleSystem::setMaterial(const std::shared_ptr<Material>& pMaterial)
{
    if (!pMaterial)
    {
        LOGLN("ParticleSystem::setMaterial: material is nullptr");
        return;
    }

    m_pMaterial = pMaterial;
    setShader(m_pMaterial->getShader());
}

void ParticleSystem::setShader(Shader* pShader)
{
    if (Window::ins->isUsingOpenGL())
    {
        m_pModelMatrixUniform = pShader->getUniformHandle("u_modelMatrix");
        m_pNodeTransformUniform = pShader->getUniformHandle("u_nodeTransform");
        m_pUseNodeTransformUniform = pShader->getUniformHandle("u_useNodeTransform");
        m_pUseTextureUniform = pShader->getUniformHandle("u_useTexture");
    }
}

void ParticleSystem::draw()
{
    if (!m_pMaterial)
    {
        return;
    }

    ASSERT(m_pMaterial->getShader(), "Material must be set before drawing the quad");

    if (m_nAliveParticleCount <= 0) return;

    if (Window::ins->isUsingOpenGL())
    {
        glBindVertexArray(m_nVertexArray);
        m_pMaterial->useShader();

        glUniform1i(m_pUseNodeTransformUniform->m_nLocation, m_bSimulateInLocal ? 1 : 0);

        glUniformMatrix4fv(m_pModelMatrixUniform->m_nLocation, 1, GL_FALSE, (const GLfloat*) getNode()->getWorldMatrix());

        int nResult = m_pMaterial->sendTexturesData();
        glUniform1i(m_pUseTextureUniform->m_nLocation, nResult);

        if (m_bSimulateInLocal)
        {
            mat4x4 nodeTransform;

            mat4x4_identity(nodeTransform);
            const Vector3& nodePosition = getNode()->getPosition();
            mat4x4_translate(nodeTransform, nodePosition.x, nodePosition.y, nodePosition.z);

            glUniformMatrix4fv(m_pNodeTransformUniform->m_nLocation, 1, GL_FALSE, (const GLfloat*) nodeTransform);
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_nInstanceBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_nAliveParticleCount * sizeof(ParticleGPUInstance), m_arrParticlesGPU);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_nAliveParticleCount);
        INCREASE_DRAW_CALL_COUNT(m_nAliveParticleCount * 2);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderEncoder->setRenderPipelineState(m_pMaterial->getShader()->getMetalPipelineState());
        pRenderEncoder->setVertexBuffer(m_pVertexBuffer, 0, 0);

        struct
        {
            mat4x4 modelMatrix;
            float useNodeTransform;
        } uniform;
        uniform.useNodeTransform = m_bSimulateInLocal ? 1.0f : 0.0f;
        memcpy(uniform.modelMatrix, getNode()->getWorldMatrix(), sizeof(mat4x4));
        pRenderEncoder->setVertexBytes(&uniform, sizeof(uniform), 2);

        pRenderEncoder->setVertexBuffer(Camera::main->getCameraMetalUBO(), 0, 3);

        memcpy(m_pInstanceBuffer->contents(), m_arrParticlesGPU, m_nAliveParticleCount * sizeof(ParticleGPUInstance));
        m_pInstanceBuffer->didModifyRange(NS::Range::Make(0, m_nAliveParticleCount * sizeof(ParticleGPUInstance)));
        pRenderEncoder->setVertexBuffer(m_pInstanceBuffer, 0, 4);

        if (Image* pImage = m_pMaterial->getImageByUniformName(SHADER_UNIFORM_TEXTURE_0); pImage)
        {
            // TODO: is there a better way to set texture index?
            pRenderEncoder->setFragmentTexture(pImage->getMetalTexture(), 0);
            pRenderEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);
        }

        pRenderEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangleStrip, NS::UInteger(0), NS::UInteger(4), NS::UInteger(m_nAliveParticleCount));
        INCREASE_DRAW_CALL_COUNT(m_nAliveParticleCount * 2);
    }
#endif // __APPLE__
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
    m_arrParticlesCPU[nIndex].m_vecVelocity[2] += m_fGravity[2] * fDeltaTime;

    m_arrParticlesGPU[nIndex].m_vecPosition[0] += m_arrParticlesCPU[nIndex].m_vecVelocity[0] * fDeltaTime;
    m_arrParticlesGPU[nIndex].m_vecPosition[1] += m_arrParticlesCPU[nIndex].m_vecVelocity[1] * fDeltaTime;
    m_arrParticlesGPU[nIndex].m_vecPosition[2] += m_arrParticlesCPU[nIndex].m_vecVelocity[2] * fDeltaTime;

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

            float vecBasePositionX, vecBasePositionY, vecBasePositionZ;
            if (m_bSimulateInLocal)
            {
                vecBasePositionX = 0;
                vecBasePositionY = 0;
                vecBasePositionZ = 0;
            }
            else
            {
                const Vector3& vecBasePosition = getNode()->getPosition();
                vecBasePositionX = vecBasePosition.x;
                vecBasePositionY = vecBasePosition.y;
                vecBasePositionZ = vecBasePosition.z;
            }

            switch (m_eSpawnShape)
            {
                case eParticleSpawnShape::DOT:
                    m_arrParticlesGPU[i].m_vecPosition[0] = vecBasePositionX;
                    m_arrParticlesGPU[i].m_vecPosition[1] = vecBasePositionY;
                    m_arrParticlesGPU[i].m_vecPosition[2] = vecBasePositionZ;
                    break;

                case eParticleSpawnShape::CIRCLE:
                    {
                        float fAngle = randomFloat(0.0f, 2.0f * M_PI);
                        float fRadius = randomFloat(0.0f, m_vecSpawnShapeSize.x);
                        m_arrParticlesGPU[i].m_vecPosition[0] = cos(fAngle) * fRadius + vecBasePositionX;
                        m_arrParticlesGPU[i].m_vecPosition[1] = sin(fAngle) * fRadius + vecBasePositionY;
                        m_arrParticlesGPU[i].m_vecPosition[2] = vecBasePositionZ;
                    }
                    break;
                
                case eParticleSpawnShape::SPHERE:
                    {
                        float fTheta = randomFloat(0.0f, 2.0f * M_PI);
                        float fPhi = randomFloat(0.0f, M_PI);
                        float fRadius = randomFloat(0.0f, m_vecSpawnShapeSize.x);
                        // TODO: Apply node's scale
                        m_arrParticlesGPU[i].m_vecPosition[0] = fRadius * sin(fPhi) * cos(fTheta) + vecBasePositionX;
                        m_arrParticlesGPU[i].m_vecPosition[1] = fRadius * sin(fPhi) * sin(fTheta) + vecBasePositionY;
                        m_arrParticlesGPU[i].m_vecPosition[2] = fRadius * cos(fPhi) + vecBasePositionZ;
                    }
                    break;

                case eParticleSpawnShape::RECTANGLE:
                case eParticleSpawnShape::BOX:
                    Vector3 oChildPos = Vector3(
                        (randomFloat(-m_vecSpawnShapeSize.x * 0.5f, m_vecSpawnShapeSize.x * 0.5f)),
                        (randomFloat(-m_vecSpawnShapeSize.y * 0.5f, m_vecSpawnShapeSize.y * 0.5f)),
                        0);
                    if (m_eSpawnShape == eParticleSpawnShape::BOX)
                    {
                        oChildPos.z = randomFloat(-m_vecSpawnShapeSize.z * 0.5f, m_vecSpawnShapeSize.z * 0.5f);
                    }
                    Vector3 oParentPos = m_pNode->transformPoint(oChildPos);
                    m_arrParticlesGPU[i].m_vecPosition[0] = oParentPos.x;
                    m_arrParticlesGPU[i].m_vecPosition[1] = oParentPos.y;
                    m_arrParticlesGPU[i].m_vecPosition[2] = oParentPos.z;
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
            m_arrParticlesCPU[i].m_vecVelocity[2] *= fStartVelocity;

            randomBetweenVec4(m_arrParticlesGPU[i].m_vecColor, m_vecStartColorMin, m_vecStartColorMax);
            m_arrParticlesGPU[i].m_fRotation = randomFloat(m_fStartRotationMin, m_fStartRotationMax);
            m_arrParticlesGPU[i].m_fScale = randomFloat(m_fStartScaleMin, m_fStartScaleMax);
            m_arrParticlesGPU[i].m_fOpacity = 1.0f;
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
    serializer.ADD_ATTRIBUTES(m_vecSpawnShapeSize);
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
    serializer.ADD_ATTRIBUTES(m_bSimulateInLocal);
    serializer.ADD_ATTRIBUTES(m_fGravity);
    
    // TODO: Store material properly

    for (int i = 0; i < 4; ++i)
    {
        const IParticleModule* pModule = m_arrParticleModules[i];
        if (pModule)
        {
            std::string strModuleDeserializeValue = pModule->getDeserializedValue();
            serializer.ADD_ATTRIBUTES_VALUE(module, strModuleDeserializeValue);
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        const IParticleIndividualModule* pModule = m_arrParticleIndividualModules[i];
        if (pModule)
        {
            std::string strModuleDeserializeValue = pModule->getDeserializedValue();
            serializer.ADD_ATTRIBUTES_VALUE(module, strModuleDeserializeValue);
        }
    }
}

bool ParticleSystem::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    DESERIALIZE_FIELD(m_nAllParticleCount);

    IF_DESERIALIZE_FIELD_CHECK(m_eSpawnShape)
    {
        m_eSpawnShape = static_cast<eParticleSpawnShape>(std::atoi(strFieldValue.data()));
        return true;
    }

    DESERIALIZE_FIELD(m_vecSpawnShapeSize);
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
    DESERIALIZE_FIELD(m_bSimulateInLocal);
    DESERIALIZE_FIELD(m_fGravity);

    // TODO: Load material properly

    IF_DESERIALIZE_FIELD_CHECK(module)
    {
        size_t pos = strFieldValue.find(":", 10);
        if (pos == std::string::npos)
        {
            return true;
        }

        std::string_view strModuleType = strFieldValue.substr(0, pos);
        std::string_view strModuleValue = strFieldValue.substr(pos + 1);

        ISerializable* pModule = TypeRegistry::instance().create(std::string(strModuleType));

        LOGLN("Deserializing module: {}, value: {}, {}", strModuleType, strModuleValue, pModule == nullptr ? "failed" : "succeeded");
        if (!pModule) { return true;}

        IParticleModule* pParticleModule = dynamic_cast<IParticleModule*>(pModule);
        if (pParticleModule)
        {
            pParticleModule->deserializeFromField(strModuleValue);
            addParticleModule(pParticleModule);
            return true;
        }
        
        IParticleIndividualModule* pParticleIndividualModule = dynamic_cast<IParticleIndividualModule*>(pModule);
        if (pParticleIndividualModule)
        {
            pParticleIndividualModule->deserializeFromField(strModuleValue);
            addParticleIndividualModule(pParticleIndividualModule);
        }

        return true;
    }

    return false;
}

void ParticleSystem::onNodeFinishedDeserialization()
{
    if (m_pMaterial)
    {
        setMaterial(m_pMaterial);
        registerBuffer();
    }
}

inline const Vector3 PARTICLE_SYSTEM_GIZMOS_COLOR = Vector3(1, 1, 1);
inline constexpr std::string_view PARTICLE_SYSTEM_GIZMOS_IMAGE = "assets/gizmos/particle.png";

inline const Color PARTICLE_SYSTEM_SPAWN_SHAPE_COLOR = Color(.45f, .58f, .75f, .5f);

void ParticleSystem::onDrawGizmos(bool bIsSelected)
{
    GizmosManager::getInstance()->addImageGizmos(this, m_pNode->getPositionInWorld(), PARTICLE_SYSTEM_GIZMOS_IMAGE, PARTICLE_SYSTEM_GIZMOS_COLOR);

    switch (m_eSpawnShape)
    {
        case eParticleSpawnShape::DOT:
            break;
        case eParticleSpawnShape::CIRCLE:
            GizmosManager::getInstance()->addCircleGizmos(
                m_pNode->getPositionInWorld(),
                m_pNode->getWorldRotationQuaternion(),
                m_vecSpawnShapeSize.x,
                PARTICLE_SYSTEM_SPAWN_SHAPE_COLOR);
            break;
        case eParticleSpawnShape::SPHERE:
            GizmosManager::getInstance()->addSphereGizmos(
                m_pNode->getPositionInWorld(),
                m_vecSpawnShapeSize.x,
                PARTICLE_SYSTEM_SPAWN_SHAPE_COLOR);
            break;
        case eParticleSpawnShape::RECTANGLE:
            GizmosManager::getInstance()->addRectangleGizmos(
                m_pNode->getPositionInWorld(),
                m_pNode->getWorldRotationQuaternion(),
                m_vecSpawnShapeSize * m_pNode->getScale(),
                PARTICLE_SYSTEM_SPAWN_SHAPE_COLOR);
            break;
        case eParticleSpawnShape::BOX:
            GizmosManager::getInstance()->addCubeGizmos(
                m_pNode->getPositionInWorld(),
                m_pNode->getWorldRotationQuaternion(),
                m_vecSpawnShapeSize * m_pNode->getScale(),
                PARTICLE_SYSTEM_SPAWN_SHAPE_COLOR);
            break;
    }
}
