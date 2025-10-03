#pragma once


#include "editor_window.h"


class HierarchyView : public IEditorWindow
{
public:
    HierarchyView();
    ~HierarchyView();

    void update(float fDeltaTime) override;

private:
    bool m_bCollapsed = false;
};