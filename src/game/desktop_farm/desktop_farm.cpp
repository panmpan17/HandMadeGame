#include "desktop_farm.h"

// #include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../../engine/core/window.h"
#include "../../engine/core/debug_macro.h"


#include "../../engine/core/debug_macro.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/core/math/random.h"
#include "../../engine/components/render/triangle.h"
#include "../../engine/components/render/quad.h"
#include "../../engine/components/render/sprite.h"
#include "../../engine/components/render/sprite_animation.h"
#include "../../engine/components/render/character2d.h"
#include "../../engine/components/render/text_renderer.h"
#include "../../engine/components/particle/particle_system.h"
#include "../../engine/components/particle/particle_spawn.h"
#include "../../engine/components/particle/particle_lifetime_change.h"
#include "../../engine/render/font/font_loader.h"
#include "../../engine/render/shader.h"
#include "../../engine/render/shader_loader.h"
#include "../../engine/render/image_loader.h"
#include "../../engine/render/skybox.h"
#include "../../engine/render/models/assimp_model_reader.h"
#include "../../engine/components/transform/rotate.h"
#include "../../engine/components/transform/movement.h"
#include "../../engine/render/material_loader.h"


DesktopFarmGame::DesktopFarmGame()
{
    ins = this;
}

void DesktopFarmGame::preconfigureWindowObject(Window* pWindow)
{
    pWindow->setResizable(false);
    pWindow->setKeepRatio(false);
    pWindow->setTransparentBackground(true);

    GLFWmonitor* pPrimaryMonitor = glfwGetPrimaryMonitor();
    int nPhysicalWidthMM, nPhysicalHeightMM;
    glfwGetMonitorPhysicalSize(pPrimaryMonitor, &nPhysicalWidthMM, &nPhysicalHeightMM);
    LOGLN("Primary Monitor: {} x {} mm", nPhysicalWidthMM, nPhysicalHeightMM);

    int x, y, width, height;
    glfwGetMonitorWorkarea(pPrimaryMonitor, &x, &y, &width, &height);
    LOGLN("Primary Monitor Work Area: {} x {} at ({}, {})", width, height, x, y);

    float scaleX, scaleY;
    glfwGetMonitorContentScale(pPrimaryMonitor, &scaleX, &scaleY);
    LOGLN("Primary Monitor Content Scale: {} x {}", scaleX, scaleY);

    pWindow->setWindowSize(width, 300);
    pWindow->setWindowPosition(x, height - 300 + y);
}

void DesktopFarmGame::setupWorldScene()
{
    {
        Shader* const pImageShader = ShaderLoader::getInstance()->getShader("image");
        Image* pCharacter = ImageLoader::getInstance()->getImageByPath("assets/images/character_animation.png");

        auto pPlayer = new Node(0.f, -1.56f, 0.f);
        pPlayer->setScale(4);
        auto pSprite = new Sprite(pCharacter, 4, 4);
        pSprite->setShader(pImageShader);
        pSprite->registerBuffer();
        pPlayer->addComponent(pSprite);

        auto pSpriteAnimation = new SpriteAnimation(pSprite);
        pSpriteAnimation->openAnimationFile("assets/character_animation.yaml");
        pPlayer->addComponent(pSpriteAnimation);

        auto pCharacter2d = new Character2d(pSpriteAnimation);
        pPlayer->addComponent(pCharacter2d);

        WorldScene::current->addNode(pPlayer);
    }
}