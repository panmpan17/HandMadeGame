#include "particle_spawn.h"
#include <sstream>


std::string ParticleIntervalSpawn::getDeserializedValue() const
{
    std::ostringstream oss;
    oss << "ParticleIntervalSpawn:" << m_fSpawnInterval;
    return oss.str();
}

void ParticleIntervalSpawn::deserializeFromField(const std::string_view& strFieldValue)
{
    m_fSpawnInterval = std::stof(strFieldValue.data());
}

std::string ParticleBurstSpawn::getDeserializedValue() const
{
    std::ostringstream oss;
    oss << "ParticleBurstSpawn:" << m_fBurstAt << "," << m_nBurstCount;
    return oss.str();
}

void ParticleBurstSpawn::deserializeFromField(const std::string_view& strFieldValue)
{
    sscanf(strFieldValue.data(), "%f,%d", &m_fBurstAt, &m_nBurstCount);
}
