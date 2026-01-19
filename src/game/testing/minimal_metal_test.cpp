#include "minimal_metal_test.h"

#include "../../engine/core/debug_macro.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/components/render/triangle.h"
#include "../../engine/components/render/quad.h"
#include "../../engine/components/render/sprite.h"
#include "../../engine/components/render/sprite_animation.h"
#include "../../engine/components/render/character2d.h"
#include "../../engine/render/shader.h"
#include "../../engine/render/shader_loader.h"
#include "../../engine/render/image_loader.h"
#include "../../engine/render/skybox.h"
#include "../../engine/components/transform/rotate.h"
#include "../../engine/components/transform/movement.h"


void firstTriangeTest()
{
    WorldScene* const pWorldScene = WorldScene::current;

    Shader* const pImageShader = ShaderLoader::getInstance()->getShader("image");

    { // Skybox
        Skybox* pSkybox = new Skybox();
        pSkybox->loadSkyboxCubmaps({
            "assets/images/skybox/right.jpg",
            "assets/images/skybox/left.jpg",
            "assets/images/skybox/top.jpg",
            "assets/images/skybox/bottom.jpg",
            "assets/images/skybox/front.jpg",
            "assets/images/skybox/back.jpg"
        });
        pWorldScene->setSkybox(pSkybox);
    }

    { // Triangle
        Shader* const pColorShared = ShaderLoader::getInstance()->getShader("colored_vertices");

        auto pNode = new Node(0.f, 0.f, 0.f);
        pNode->setRotationQuaternion(Quaternion::fromEulerAngles({0.f, 0.f, 0.f}));

        auto pTriangle = new Triangle();
        pTriangle->setShader(pColorShared);
        pTriangle->registerBuffer();
        pNode->addComponent(pTriangle);

        pNode->addComponent(new Rotate3D(0, 0, -10.0f));

        pNode->addComponent(new Movement(1.0f)); // Add movement component with speed 1.0f

        pWorldScene->addNode(pNode);
    }

    { // Quad with Image
        Image* pTestImage = ImageLoader::getInstance()->getImageByPath("assets/images/test.png");

        auto pNode2 = new Node(0.5f, 0.5f, 0.f);
        pNode2->setScale(2);

        vec4 red = {1.f, 0.f, 0.f, .5f}; // Red color for the quad
        auto pQuad = new Quad(0.5f, 0.5f, red);
        pQuad->setShader(pImageShader);
        pQuad->setImage(pTestImage);
        pQuad->registerBuffer();
        pNode2->addComponent(pQuad);

        // pNode2->addComponent(new Rotate3D(0, 0, 1.0f));

        pWorldScene->addNode(pNode2);
    }

    {
        Image* pCharacter = ImageLoader::getInstance()->getImageByPath("assets/images/character_animation.png");

        auto pPlayer = new Node(0.f, -1.f, 0.f);
        auto pSprite = new Sprite(pCharacter, 4, 4);
        pSprite->setShader(pImageShader);
        pSprite->registerBuffer();
        pPlayer->addComponent(pSprite);

        auto pSpriteAnimation = new SpriteAnimation(pSprite);
        pSpriteAnimation->openAnimationFile("assets/character_animation.yaml");
        pPlayer->addComponent(pSpriteAnimation);

        auto pCharacter2d = new Character2d(pSpriteAnimation);
        pPlayer->addComponent(pCharacter2d);

        pWorldScene->addNode(pPlayer);
    }
}