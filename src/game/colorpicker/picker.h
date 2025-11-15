#pragma once

#include "../../editor/editor_window.h"

class Window;
class Quad;

class ColorPicker
{
public:
    static ColorPicker* getInstance() { return ins; }

    ColorPicker()
    {
        ins = this;
    }

    void preconfigureWindowObject(Window* pWindow);
    void pickerMain();

private:
    static inline ColorPicker* ins = nullptr;

    Window* m_pWindow = nullptr;
};


class PickerInspector : public IEditorWindow
{
public:
    PickerInspector() {}
    ~PickerInspector() {}

    void update() override;
};