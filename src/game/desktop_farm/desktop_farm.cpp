#include "desktop_farm.h"

// #include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "farm_animal.h"
#include "music_player.h"

#include "../../engine/core/window.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/core/math/random.h"
#include "../../engine/core/audio/audio_engine.h"
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


#if VSCODE_ONLY
void glfwGetMonitorWorkarea(GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height);
#endif // VSCODE_ONLY

constexpr int WINDOW_HEIGHT = 300;


DesktopFarmGame::DesktopFarmGame()
{
    ins = this;
}

void DesktopFarmGame::preconfigureWindowObject(Window* pWindow)
{
    pWindow->setResizable(false);
    pWindow->setKeepRatio(false);
    pWindow->setTransparentBackground(true);

    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    GLFWmonitor* pPrimaryMonitor = glfwGetPrimaryMonitor();
    int nWorkAreaOffsetX, nWorkAreaOffsetY, nWorkAreaWidth, nWorkAreaHeight;
    glfwGetMonitorWorkarea(pPrimaryMonitor, &nWorkAreaOffsetX, &nWorkAreaOffsetY, &nWorkAreaWidth, &nWorkAreaHeight);

    pWindow->setWindowSize(nWorkAreaWidth, WINDOW_HEIGHT);
    pWindow->setWindowPosition(nWorkAreaOffsetX, nWorkAreaHeight - WINDOW_HEIGHT + nWorkAreaOffsetY);
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

        auto pFarmAnimal = new FarmAnimal(pSpriteAnimation);
        pPlayer->addComponent(pFarmAnimal);

        WorldScene::current->addNode(pPlayer);
    }

    {
        auto pNode = new Node(0.f, 0.f, 0.f);
        MusicPlayer* pMusicPlayer = new MusicPlayer();

        auto pMusic1 = std::make_shared<AudioClip>("assets/musics/lofidreams-lofi-jazz-music-485312.mp3");
        auto pMusic2 = std::make_shared<AudioClip>("assets/musics/idoberg-cozy-lofi-beat-split-memmories-248205.mp3");

        pMusicPlayer->addAudioClip(pMusic1);
        pMusicPlayer->addAudioClip(pMusic2);

        pMusicPlayer->play();

        pNode->addComponent(pMusicPlayer);
        WorldScene::current->addNode(pNode);
    }
}