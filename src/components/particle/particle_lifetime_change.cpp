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

