#include "component.h"
#include "../serializer.h"

void Component::serializedTo(DataSerializer& serializer) const
{
    serializer.startClassHeader(getTypeName());
    
    serializeToWrapper(serializer);

    serializer.endClassHeader();
}

void Component::deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
}
