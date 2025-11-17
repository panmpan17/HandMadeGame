#include "picker.h"

#include <cstdlib>
#include <string>
#include <format>
// #include "paddle_control.h"
// #include "pong.h"
// #include "../../engine/core/scene/node.h"
#include "../../engine/core/debug_macro.h"
#include "../../engine/core/camera.h"
#include "../../engine/core/window.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/core/input/input_manager.h"
// #include "../../engine/core/math/random.h"
// #include "../../engine/render/image.h"
// #include "../../engine/render/shader.h"
#include "../../engine/render/shader_loader.h"
#include "../../engine/components/render/quad.h"
// #include "../../engine/components/render/triangle.h"
// #include "../../engine/components/render/character2d.h"
// #include "../../engine/components/transform/rotate.h"
// #include "../../engine/components/transform/movement.h"
// #include "../../engine/components/particle/particle_system.h"
// #include "../../engine/components/particle/particle_spawn.h"
// #include "../../engine/components/particle/particle_lifetime_change.h"



#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


Quad* g_pColorDisplayQuad = nullptr;

void copyToClipboardShell(const std::string& text)
{
    std::string command = "echo \"" + text + "\" | tr -d '\n' | pbcopy";
    std::system(command.c_str());
    
}

void ColorPicker::preconfigureWindowObject(Window* pWindow)
{
    m_pWindow = pWindow;
    pWindow->setPostProcessEnabled(false);
    pWindow->setAddGameRelatedIMGUIWindows(false);
    pWindow->setShowFPS(false);
    pWindow->setResizable(false);
    pWindow->setWindowSize(400, 400);
    pWindow->addEditorWindow(new PickerInspector());
}

void ColorPicker::pickerMain()
{
    WorldScene* const pWorldScene = WorldScene::current;


    // WorldScene* const pWorldScene = WorldScene::current;
    // pWorldScene->setInitEditorCamera(false);

    {
        auto pCameraNode = new Node(0.f, 0.f, 2.5f);
        pCameraNode->setName("Camera");
        Camera* pCamera = new Camera();
        pCamera->useAsMain();
        pCamera->setUseOrthoProjection(true);
        pCamera->setWorldSizeScale(.5f);
        pCameraNode->addComponent(pCamera);
        pWorldScene->addNode(pCameraNode);
    }

    Shader* pImageShader = ShaderLoader::getInstance()->getShader("image");

    {// Canvas
        auto pCenterLine = new Node();
        pCenterLine->setPosition(0.f, 0.f);

        g_pColorDisplayQuad = new Quad(1.f, 1.f, vec4{1.f, 1.f, 1.f, 1.f});
        g_pColorDisplayQuad->setShader(pImageShader);
        g_pColorDisplayQuad->registerBuffer();
        pCenterLine->addComponent(g_pColorDisplayQuad);

        pWorldScene->addNode(pCenterLine);
    }

    InputManager* pInputManager = InputManager::getInstance();
    pInputManager->registerKeyPressCallback(KeyCode::KEY_C, [pInputManager](bool pressed) {
        bool bControl = (pInputManager->isKeyPressed(KeyCode::KEY_LEFT_CONTROL) || pInputManager->isKeyPressed(KeyCode::KEY_RIGHT_CONTROL)
                         || pInputManager->isKeyPressed(KeyCode::KEY_LEFT_META) || pInputManager->isKeyPressed(KeyCode::KEY_RIGHT_META));
        if (pressed && bControl)
        {
            vec4 color;
            g_pColorDisplayQuad->getColor(color);
            std::string strContent = std::format("vec3({:.2f}, {:.2f}, {:.2f})", color[0], color[1], color[2]);
            copyToClipboardShell(strContent);
            // int a = static_cast<int>(color[3] * 255.f);
            // char szColorCode[16];
            // snprintf(szColorCode, sizeof(szColorCode), "#%02X%02X%02X%02X", r, g, b, a);
            // glfwSetClipboardString(Window::getInstance()->getGLFWwindow(), szColorCode);
        }
    });
}

void PickerInspector::update()
{
    bool bOpen = true;
    ImGui::Begin("##picker", &bOpen);
    ImGui::SetWindowSize(ImVec2(200, 400), ImGuiCond_Always);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);

    vec4 color;
    g_pColorDisplayQuad->getColor(color);
    if (ImGui::ColorEdit3("Color", color))
    {
        g_pColorDisplayQuad->setColor(color);
    }

    ImGui::End();
}