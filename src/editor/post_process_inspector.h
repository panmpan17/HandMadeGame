#pragma once


#include "editor_window.h"


class IRenderProcess;


class PostProcessInspector : public IEditorWindow
{
public:
    PostProcessInspector();
    ~PostProcessInspector();

    void update(float fDeltaTime) override;
    void updateRenderProcessQueue(IRenderProcess* pProcess);

private:
    bool m_bCollapsed = false;
};
