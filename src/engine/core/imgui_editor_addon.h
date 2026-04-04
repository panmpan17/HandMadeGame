#pragma once

#include "../../utils/expandable_array.h"
#include "../../editor/editor_window.h"

#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__


class Window;


class ImGuiEditorAddon
{
public:
    ImGuiEditorAddon(Window* pWindow);
    ~ImGuiEditorAddon();

    inline void addEditorWindow(IEditorWindow* pWindow) { m_oEditorWindows.addElement(pWindow); }

    void startIMGUIFrame();
    void update();
    void renderFrame();

    inline void setAddGameRelatedIMGUIWindows(bool bAdd) { m_bAddGameRelatedIMGUIWindows = bAdd; }

    inline void setShowFrameInfo(bool bShow) { m_bShowFrameInfo = bShow; }
    inline bool getShowFrameInfo() const { return m_bShowFrameInfo; }

private:
    Window* m_pWindow = nullptr;
    bool m_bIMGUIInitialized = false;
    bool m_bActive = true;

    bool m_bAddGameRelatedIMGUIWindows = true;
    bool m_bShowFrameInfo = true;

    PointerExpandableArray<IEditorWindow*> m_oEditorWindows = PointerExpandableArray<IEditorWindow*>(2);

#if __APPLE__
    MTL::RenderPassDescriptor* m_pRenderPassDescriptor = nullptr;
#endif // __APPLE__

    void setupIMGUIAndEditorWindows();

    void drawEditorWindows();
    void drawFrameInfo();
};