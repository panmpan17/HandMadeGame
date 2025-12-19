#include "scaling.h"

#include "../../../editor/inspector_helper.h"


void TwoPointScaling::onInspectorUI(int nComponentIndex)
{
    inspectorVector3Field(nComponentIndex, "Start", reinterpret_cast<Vector3&>(m_vecStart));
    inspectorVector3Field(nComponentIndex, "End", reinterpret_cast<Vector3&>(m_vecEnd));
    inspectorFloatField(nComponentIndex, "Duration", m_fDuration);
}