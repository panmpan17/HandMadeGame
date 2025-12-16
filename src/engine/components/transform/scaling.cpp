#include "scaling.h"

#include "../../../editor/inspector_helper.h"


void TwoPointScaling::onInspectorUI(int nComponentIndex)
{
    VECTOR3_FIELD(nComponentIndex, "Start", m_vecStart);
    VECTOR3_FIELD(nComponentIndex, "End", m_vecEnd);
    FLOAT_FIELD(nComponentIndex, "Duration", m_fDuration);
}