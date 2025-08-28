#include "rotate.h"
#include "../serializer.h"

void Rotate::deserializeField(const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    DESERIALIZE_FIELD(m_fRotationSpeed);
}
