#pragma once

// #include "../component.h"
#include "../drawable_interface.h"
#include <linmath.h>
#include <functional>

typedef unsigned int GLuint;
class Image;

struct ParticleGPUInstance
{
    vec2 m_vecPosition;
    vec4 m_vecColor;
    float m_fRotation;
    float m_fScale;
    float m_fOpacity;
};

struct ParticleCPUInstance
{
    float m_fRotationSpeed;
    vec2 m_vecVelocity;
    float m_fLifetime;
    float m_fMaxLifetime;
    float m_fBaseScale;

    bool isAlive() const { return m_fLifetime > 0; }
};


class ParticleSystem;
class IParticleModule : public ISerializable
{
public:
    virtual ~IParticleModule() = default;

    virtual void update(ParticleSystem& particleSystem, float deltaTime) = 0;

    virtual void onActiveTimeReset() {}

    virtual std::string getDeserializedValue() const { return typeid(*this).name(); }
    virtual void deserializeFromField(const std::string_view& strFieldValue) = 0;

    virtual void serializedTo(DataSerializer& serializer) const override {};
    virtual bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) override { return false; }
};

class IParticleIndividualModule : public ISerializable
{
public:
    virtual ~IParticleIndividualModule() = default;

    virtual void update(ParticleSystem& particleSystem, ParticleGPUInstance* pParticleGpu, ParticleCPUInstance* pParticleCpu, float deltaTime) = 0;

    virtual std::string getDeserializedValue() const { return typeid(*this).name(); }
    virtual void deserializeFromField(const std::string_view& strFieldValue) = 0;

    virtual void serializedTo(DataSerializer& serializer) const override {};
    virtual bool deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue) override { return false; }
};


enum class eParticleSpawnShape : int
{
    DOT,
    CIRCLE,
    BOX,
};

typedef std::function<void(vec2&)> ParticleStartVelocityDirectionOverride;


class ParticleSystem : public IDrawable
{
public:
    ParticleSystem(int nParticleCount = 100, bool bSimulateInLocal = false);

    ~ParticleSystem() override;

    bool isUpdatable() const override { return true; }

    
    void registerBuffer() override;
    void draw() override;
    void setShader(Shader* pShader) override;

    void update(float fDeltaTime) override;

    /// @brief Spawns new particles
    /// @param nSpawnCount The number of particles to spawn
    void spawnNewParticles(int nSpawnCount = 0);

    void addParticleModule(IParticleModule* pModule) { for (int i = 0; i < 4; ++i) { if (m_arrParticleModules[i] == nullptr) { m_arrParticleModules[i] = pModule; break; } } }
    void addParticleIndividualModule(IParticleIndividualModule* pModule) { for (int i = 0; i < 4; ++i) { if (m_arrParticleIndividualModules[i] == nullptr) { m_arrParticleIndividualModules[i] = pModule; break; } } }

    void setParticleLifetime(float fMin, float fMax) { m_fLifetimeMin = fMin; m_fLifetimeMax = fMax; }
    void setParticleStartRotation(float fMin, float fMax) { m_fStartRotationMin = fMin; m_fStartRotationMax = fMax; }
    void setParticleStartRotationSpeed(float fMin, float fMax) { m_fStartRotationSpeedMin = fMin; m_fStartRotationSpeedMax = fMax; }
    void setParticleStartScale(float fMin, float fMax) { m_fStartScaleMin = fMin; m_fStartScaleMax = fMax; }
    void setParticleStartColor(const vec4& colorMin, const vec4& colorMax) { vec4_dup(m_vecStartColorMin, colorMin); vec4_dup(m_vecStartColorMax, colorMax); }
    void setParticleStartVelocity(float fMin, float fMax) { m_fStartVelocityMin = fMin; m_fStartVelocityMax = fMax; }
    void setParticleStartVelocityDirectionOverride(ParticleStartVelocityDirectionOverride funcOverride) { m_funcStartVelocityDirectionOverride = funcOverride; }

    void setSpawnShape(eParticleSpawnShape eShape) { m_eSpawnShape = eShape; }
    void setSpawnShapeDimensions(float fWidth, float fHeight) { m_fSpawnShapeWidth = fWidth; m_fSpawnShapeHeight = fHeight; }

    void setGravity(const vec2& vecGravity) { vec2_dup(m_fGravity, vecGravity); }
    void setGravity(float fX, float fY) { m_fGravity[0] = fX; m_fGravity[1] = fY; }

    void setImage(Image* pImage) { m_pImage = pImage; }

    void play(bool bResetActiveTimer = true)
    {
        m_bIsEmitting = true;
        if (bResetActiveTimer)
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
    }
    void stop() { m_bIsEmitting = false; }
    bool getIsEmitting() const { return m_bIsEmitting; }

    void setActiveTime(float fTime) { m_fActiveTime = fTime; }
    float getActiveTimer() const { return m_fActiveTimer; }

    void setIsLooping(bool bLooping) { m_bIsLooping = bLooping; }

private:
    ParticleGPUInstance* m_arrParticlesGPU = nullptr;
    ParticleCPUInstance* m_arrParticlesCPU = nullptr;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;
    GLuint m_nInstanceBuffer = 0;
    GLuint m_nUseNodeTransformUniform, m_nUseTextureUniform, m_nMVPUniForm, m_nNodeTransformUniform, m_nTextureUniform;

    Shader* m_pShader = nullptr;

    int m_nAllParticleCount = 0;
    int m_nAliveParticleCount = 0;
    int m_nLastAliveParticleIndex = 0;

    eParticleSpawnShape m_eSpawnShape = eParticleSpawnShape::DOT;
    float m_fSpawnShapeWidth = 0.1f;
    float m_fSpawnShapeHeight = 0.1f;

    float m_fLifetimeMin = 1.0f;
    float m_fLifetimeMax = 3.0f;

    float m_fStartRotationMin = 0.0f;
    float m_fStartRotationMax = 0.0f;
    float m_fStartRotationSpeedMin = 0.0f;
    float m_fStartRotationSpeedMax = 0.0f;

    float m_fStartScaleMin = 0.2f;
    float m_fStartScaleMax = 0.4f;

    vec4 m_vecStartColorMin = { 0.9f, 0.9f, 0.9f, 1.f };
    vec4 m_vecStartColorMax = { 1.f, 1.f, 1.f, 1.f };

    float m_fStartVelocityMin = 0.3f;
    float m_fStartVelocityMax = 0.5f;
    ParticleStartVelocityDirectionOverride m_funcStartVelocityDirectionOverride = nullptr;

    vec2 m_fGravity = { 0.0f, -0.981f };

    bool m_bSimulateInLocal = false;

    Image* m_pImage = nullptr;

    bool m_bIsEmitting = true;
    bool m_bIsLooping = true;
    float m_fActiveTime = 5.0f;
    float m_fActiveTimer = 0.0f;


    IParticleModule* m_arrParticleModules[4];
    IParticleIndividualModule* m_arrParticleIndividualModules[4];

    void updateParticle(int& nIndex, float fDeltaTime);
    void sortAliveParticleInFront();

    COMPONENT_REGISTER_SERIALIZABLE(ParticleSystem)
};

REGISTER_CLASS(ParticleSystem)
