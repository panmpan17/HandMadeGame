#include "imgui_editor_addon.h"

#include "window.h"
#include "input/input_manager.h"
#include "../render/core/renderer.h"
#include "../misc/preference.h"
#include "../../editor/node_inspector.h"
#include "../../editor/hierarchy_view.h"
#include "../../editor/post_process_inspector.h"


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#if __APPLE__
#include "imgui_impl_metal.h"
#endif // __APPLE__


ImGuiEditorAddon::ImGuiEditorAddon(Window* pWindow)
    : m_pWindow(pWindow)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    if (Renderer::isUsingOpenGL())
    {
        ImGui_ImplGlfw_InitForOpenGL(m_pWindow->getGLFWwindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        ImGui_ImplGlfw_InitForOther(m_pWindow->getGLFWwindow(), true);
        ImGui_ImplMetal_Init(m_pWindow->getMetalDevice());

        m_pRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

        MTL::RenderPassColorAttachmentDescriptor* pColorAttachment = m_pRenderPassDescriptor->colorAttachments()->object(0);
        pColorAttachment->setLoadAction(MTL::LoadActionLoad);
        pColorAttachment->setStoreAction(MTL::StoreActionStore);
    }
#endif // __APPLE__

    InputManager::getInstance()->registerKeyPressCallback(KeyCode::KEY_FUNCTION_3, [this](bool pressed) {
        if (pressed)
        {
            m_bActive = !m_bActive;
            Preference::setEnableIMGUI(m_bActive);
        }
    });

    setupIMGUIAndEditorWindows();
}


ImGuiEditorAddon::~ImGuiEditorAddon()
{
    if (Renderer::isUsingOpenGL())
    {
        ImGui_ImplOpenGL3_Shutdown();
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        ImGui_ImplMetal_Shutdown();

        if (m_pRenderPassDescriptor)
        {
            m_pRenderPassDescriptor->release();
            m_pRenderPassDescriptor = nullptr;
        }
    }
#endif // __APPLE__
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void ImGuiEditorAddon::setupIMGUIAndEditorWindows()
{
    if (m_bAddGameRelatedIMGUIWindows)
    {
        m_oEditorWindows.addElement(new NodeInspector());
        m_oEditorWindows.addElement(new HierarchyView());
        m_oEditorWindows.addElement(new PostProcessInspector());
    }

    for (int i = 0; i < m_oEditorWindows.getCount(); ++i)
    {
        IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
        if (pWindow)
        {
            bool bActive = Preference::getPlayerPreferenceInstance().getBool(std::string("EditorWindow_") + typeid(*pWindow).name(), true);
            pWindow->setActive(bActive);
        }
    }
}


void ImGuiEditorAddon::startIMGUIFrame()
{
    if (Renderer::isUsingOpenGL())
    {
        ImGui_ImplOpenGL3_NewFrame();
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        m_pRenderPassDescriptor->colorAttachments()->object(0)->setTexture(m_pWindow->getCurrentDrawable()->texture());

        ImGui_ImplMetal_NewFrame(m_pRenderPassDescriptor);
    }
#endif // __APPLE__

    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiEditorAddon::update()
{
    if (!m_bActive)
    {
        return;
    }

    if (m_bShowFrameInfo)
    {
        drawFrameInfo();
    }

    drawEditorWindows();
}

void ImGuiEditorAddon::drawEditorWindows()
{
    int nSize = m_oEditorWindows.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
        if (pWindow && pWindow->isActive())
        {
            pWindow->update();
        }
    }

    // TODO: try to fix the issue of menu bar on Metal will crash
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Editor Windows"))
        {
            for (int i = 0; i < nSize; ++i)
            {
                IEditorWindow* pWindow = m_oEditorWindows.getElement(i);
                if (pWindow)
                {
                    if (ImGui::MenuItem(typeid(*pWindow).name(), NULL, pWindow->isActive()))
                    {
                        pWindow->setActive(!pWindow->isActive());
                        Preference::getPlayerPreferenceInstance().setBool(std::string("EditorWindow_") + typeid(*pWindow).name(), pWindow->isActive());
                    }
                }
            }

            ImGui::EndMenu();
        }

        bool bDrawGizmos = Window::ins->getDrawGizmos();
        if (ImGui::MenuItem("Show Gizmos", NULL, bDrawGizmos))
        {
            bDrawGizmos = !bDrawGizmos;
            Window::ins->setDrawGizmos(bDrawGizmos);
            Preference::setEnableGizmos(bDrawGizmos);
        }

        ImGui::EndMainMenuBar();
    }
}

void ImGuiEditorAddon::drawFrameInfo()
{
    ImGui::Begin("Info", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar
                 | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
                 | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowSize(ImVec2(200, 100), ImGuiCond_Always);
    ImGui::SetWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 60), ImGuiCond_Always);
    ImGui::Text("Grahpics: %s", Renderer::isUsingOpenGL() ? "OpenGL" : (Renderer::isUsingMetal() ? "Metal" : "Unknown"));
    ImGui::Text("%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Draw Call: %d; Triangle: %d", Renderer::getDrawCallCount(), Renderer::getTriangleCount());
    ImGui::End();
}

void ImGuiEditorAddon::renderFrame()
{
    ImGui::Render();

    if (Renderer::isUsingOpenGL())
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::CommandBuffer* pCurrentCommandBuffer = m_pWindow->getCurrentCommandBuffer();
        MTL::RenderCommandEncoder* pIMGUIRenderCommandEncoder = pCurrentCommandBuffer->renderCommandEncoder(m_pRenderPassDescriptor);
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), pCurrentCommandBuffer, pIMGUIRenderCommandEncoder);
        pIMGUIRenderCommandEncoder->endEncoding();
    }
#endif // __APPLE__
}
