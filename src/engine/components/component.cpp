#include "component.h"
#include "../serialization/serializer.h"

void Component::serializedTo(DataSerializer& serializer) const
{
    serializer.startClassHeader(getTypeName());
    
    serializer.ADD_ATTRIBUTES(m_nID);
    serializeToWrapper(serializer);

    serializer.endClassHeader();
}

bool Component::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    DESERIALIZE_FIELD(m_nID);

    return false;
}
