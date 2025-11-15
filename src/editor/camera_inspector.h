#pragma once

#include "editor_window.h"


class CameraInspector : public IEditorWindow
{
public:
    CameraInspector();
    ~CameraInspector();

    void update() override;

private:
    bool m_bCollapsed = false;
};
