#include "rotate.h"
#include "../serializer.h"

bool Rotate::deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    DESERIALIZE_FIELD(m_fRotationSpeed);

    return false;
}

void Rotate::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_fRotationSpeed);
}

void Rotate::onNodeFinishedDeserialization() {}
