#include "minimal_metal_test.h"

#include "../../engine/core/debug_macro.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/core/math/random.h"
#include "../../engine/components/render/triangle.h"
#include "../../engine/components/render/quad.h"
#include "../../engine/components/render/sprite_9slice.h"
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


void addTestSkybox()
{
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
        WorldScene::current->setSkybox(pSkybox);
    }
}

void firstTriangeTest()
{
    WorldScene* const pWorldScene = WorldScene::current;

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
}

void addQuadImage()
{
    Shader* const pImageShader = ShaderLoader::getInstance()->getShader("image");
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

        WorldScene::current->addNode(pNode2);
    }
}

void addSpriteAnimation()
{
    {
        Shader* const pImageShader = ShaderLoader::getInstance()->getShader("image");
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

        WorldScene::current->addNode(pPlayer);
    }
}

void addParticles()
{
    { // Particle
        std::shared_ptr<Material> pMaterial = MaterialLoader::getInstance()->getMaterial("assets/materials/dust_particle.yaml");

        Node* pNode5 = new Node(0, 0, 0);
        pNode5->setName("Smoke");

        ParticleSystem* particle = new ParticleSystem(100, false);
        particle->setMaterial(pMaterial);
        particle->registerBuffer();
        particle->setParticleStartColor({ 1.f, 1.f, 1.f, 1.f }, { 0.9f, 0.9f, 0.9f, 1.f });
        particle->setIsLooping(true);
        particle->addParticleModule(new ParticleIntervalSpawn(10));
        particle->addParticleIndividualModule(new ScaleThroughParticleLifetime(0.1f, 1.f));
        particle->setParticleLifetime(4, 6);
        particle->setParticleStartVelocity(1, 1);
        particle->setGravity(0, 0, 0);
        particle->setSpawnShape(eParticleSpawnShape::BOX);
        particle->setSpawnShapeDimensions(.1f, .1f, .1f);
        particle->setParticleStartVelocityDirectionOverride([](vec3& velocity) {
            velocity[0] = randomFloat(-0.05f, 0.05f); // Override X direction
            velocity[1] = randomFloat(-0.05f, 0.05f); // Override Y direction
            velocity[2] = randomFloat(-0.05f, 0.05f); // Override Z direction
        });

        pNode5->addComponent(particle);

        pNode5->addComponent(new TwoPointsMovement({ -0.5f, 0.f, 1.f }, { 0.5f, 0.f, 1.f }, 2.0f));

        pNode5->setActive(false);
        WorldScene::current->addNode(pNode5);
    }
}

void add3DBox()
{
    std::shared_ptr<Material> pMaterial = MaterialLoader::getInstance()->getMaterial("assets/materials/box.yaml");
    AssimpModelReader oModelReader("assets/models/box.obj", { pMaterial });
    Node* pBox = oModelReader.instantiateCloneNode();
    pBox->setPosition(0.f, 0.f, 0.f);
    WorldScene::current->addNode(pBox);
}

void fontTest()
{
    WorldScene* const pWorldScene = WorldScene::current;

    {
        Node* pTextNode = new Node(0.f, 0.f, 0.f);

        Font* pFont = FontLoader::getInstance()->getFont("assets/fonts/Arial Unicode.ttf");

        Shader* pTextShader = ShaderLoader::getInstance()->getShader("text");
        TextRenderer* pTextRenderer = new TextRenderer(pFont);
        pTextRenderer->setText(u"我是潘啟元 Hello, World!");
        pTextRenderer->setShader(pTextShader);

        pTextNode->addComponent(pTextRenderer);
        pWorldScene->addNode(pTextNode);
    }
}

void sprite9SliceTest()
{
    WorldScene* const pWorldScene = WorldScene::current;

    {
        Image* pTestImage = ImageLoader::getInstance()->getImageByPath("assets/images/test_9slice.png");
        Node* pSprite9SliceNode = new Node(0.f, 0.f, 0.f);
        Sprite9Slice* pSprite9Slice = new Sprite9Slice(pTestImage, 10.f, 2.f, 100.f, { 20.f, 20.f, 20.f, 20.f });
        pSprite9Slice->setShader(ShaderLoader::getInstance()->getShader("sprite_9slice"));
        // pSprite9Slice->registerBuffer();
        pSprite9SliceNode->addComponent(pSprite9Slice);
        pWorldScene->addNode(pSprite9SliceNode);
    }
}

void fullTest()
{
    // addTestSkybox();
    // firstTriangeTest();
    // addQuadImage();
    // addSpriteAnimation();
    // addParticles();
    // add3DBox();
    // fontTest();
    sprite9SliceTest();
}
