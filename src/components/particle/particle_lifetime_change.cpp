#include "particle_lifetime_change.h"
#include <sstream>

std::string ScaleThroughParticleLifetime::getDeserializedValue() const
{
    std::ostringstream oss;
    oss << "ScaleThroughParticleLifetime:" << m_fMinScale << "," << m_fMaxScale;
    return oss.str();
}

void ScaleThroughParticleLifetime::deserializeFromField(const std::string_view& strFieldValue)
{
    sscanf(strFieldValue.data(), "%f,%f", &m_fMinScale, &m_fMaxScale);
}



std::string OpacityThroughParticleLifetime::getDeserializedValue() const
{
    std::ostringstream oss;
    oss << "OpacityThroughParticleLifetime:" << m_fMinOpacity << "," << m_fMaxOpacity;
    return oss.str();
}

void OpacityThroughParticleLifetime::deserializeFromField(const std::string_view& strFieldValue)
{
    sscanf(strFieldValue.data(), "%f,%f", &m_fMinOpacity, &m_fMaxOpacity);
}

