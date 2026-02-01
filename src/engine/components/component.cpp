#include "component.h"
#include "../core/serialization/serializer.h"

void NodeComponent::serializedTo(DataSerializer& serializer) const
{
    serializer.startClassHeader(getTypeName());
    
    serializer.ADD_ATTRIBUTES(m_nID);
    serializeToWrapper(serializer);

    serializer.endClassHeader();
}

bool NodeComponent::deserializeField(DataDeserializer& deserializer, std::string_view strFieldName, std::string_view strFieldValue)
{
    DESERIALIZE_FIELD(m_nID);

    return false;
}
