#include "rotate.h"
#include "../../core/serialization/serializer.h"

bool Rotate3D::deserializeField(DataDeserializer& deserializer, std::string_view strFieldName, std::string_view strFieldValue)
{
    if (NodeComponent::deserializeField(deserializer, strFieldName, strFieldValue)) return true;

    DESERIALIZE_FIELD(m_vecCurrentEulerAngles);

    return false;
}

void Rotate3D::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_vecCurrentEulerAngles);
}
