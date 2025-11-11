#include "test.h"

#include "../../engine/core/camera.h"
#include "../../engine/core/window.h"
#include "../../engine/core/serialization/serializer.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/core/math/random.h"
#include "../../engine/render/image.h"
#include "../../engine/render/shader.h"
#include "../../engine/render/shader_loader.h"
#include "../../engine/render/material.h"
#include "../../engine/render/skybox.h"
#include "../../engine/render/models/assimp_model_reader.h"
#include "../../engine/render/lighting/direction_light.h"
#include "../../engine/render/lighting/point_light.h"
#include "../../engine/render/post_process/render_process_queue.h"
#include "../../engine/render/post_process/bloom_test.h"
#include "../../engine/render/post_process/order_dithering.h"
#include "../../engine/render/post_process/difference_of_gaussian.h"
#include "../../engine/render/post_process/gamma_correction.h"
#include "../../engine/components/render/quad.h"
#include "../../engine/components/render/triangle.h"
#include "../../engine/components/render/character2d.h"
#include "../../engine/components/transform/rotate.h"
#include "../../engine/components/transform/movement.h"
#include "../../engine/components/particle/particle_system.h"
#include "../../engine/components/particle/particle_spawn.h"
#include "../../engine/components/particle/particle_lifetime_change.h"


void setupPostProcess()
{
    RenderProcessQueue* pQueue;
    if (!Window::ins || !(pQueue = Window::ins->getRenderProcessQueue()))
    {
        return;
    }

    auto pGammaCorrection = new GammaCorrection(pQueue);
    pQueue->addProcess(pGammaCorrection);

    auto pDifferenceOfGaussian = new DifferenceOfGaussian(pQueue);
    pQueue->addProcess(pDifferenceOfGaussian);

    auto pOrderDithering = new OrderDithering(pQueue);
    pQueue->addProcess(pOrderDithering);

    auto pBloomTest = new BloomTest(pQueue);
    pQueue->addProcess(pBloomTest);
}

void createDemo1()
{
    WorldScene* const pWorldScene = WorldScene::current;

    // TODO: remove this
    DataSerializer oSerializer("assets/level.txt");

    Shader* pTestShader = ShaderLoader::getInstance()->getShader("test");
    Shader* pImageShader = ShaderLoader::getInstance()->getShader("image");
    Shader* pParticleShader = ShaderLoader::getInstance()->getShader("particle_instance");

    Image* pTestImage = ImageLoader::getInstance()->getImageByPath("assets/images/test.png");
    Image* pDustImage = ImageLoader::getInstance()->getImageByPath("assets/images/dust_1.png");

    { // Triangle
        auto pNode = new Node(-0.5f, 0.f, 0.f);

        auto pTriangle = new Triangle();
        pTriangle->setShader(pTestShader);
        pTriangle->registerBuffer();
        pNode->addComponent(pTriangle);

        pNode->addComponent(new Rotate3D(0, 0, -10.0f));

        pNode->addComponent(new Movement(1.0f)); // Add movement component with speed 1.0f

        pWorldScene->addNode(pNode);

        oSerializer << pNode;
    }

    {
        auto pNode = new Node(-0.4f, 0.f, 0.f);

        auto pTriangle = new Triangle();
        pTriangle->setShader(pTestShader);
        pTriangle->registerBuffer();
        pNode->addComponent(pTriangle);

        pWorldScene->addNode(pNode);

        oSerializer << pNode;
    }

    // Quads 1
    
    auto pNode2 = new Node(0.5f, 0.5f, 0.f);

    vec4 red = {1.f, 0.f, 0.f, 1.f}; // Red color for the quad
    auto pQuad = new Quad(0.5f, 0.5f, red);
    pQuad->setShader(pImageShader);
    pQuad->setImage(pTestImage);
    pQuad->registerBuffer();
    pNode2->addComponent(pQuad);

    pNode2->addComponent(new Rotate3D(0, 0, 1.0f));

    pWorldScene->addNode(pNode2);

    oSerializer << pNode2;

    // Quads 2
    auto pNode3 = new Node(0.5f, 0.5f, 0.f);

    vec4 color = {0.5f, 0.5f, 1.f, 1.f}; // Blue color for the second quad
    auto pQuad2 = new Quad(0.3f, 0.3f, color);
    pQuad2->setShader(pImageShader);
    pQuad2->setImage(pDustImage);
    pQuad2->registerBuffer();
    pNode3->addComponent(pQuad2);

    pWorldScene->addNode(pNode3);

    oSerializer << pNode3;

    // Particle System
    // auto pNode4 = new Node(0, 0, 0, 0);
    // auto particle = new SimpleParticleSystem(10000);
    // particle->setShader(ShaderLoader::getInstance()->getShader("particle"));
    // particle->registerBuffer();
    // pNode4->addComponent(particle);
    // addNode(pNode4);

    auto pNode5 = new Node(0, 0, 0);
    auto particle2 = new ParticleSystem(100, false);
    particle2->setImage(pDustImage);
    particle2->setShader(pParticleShader);
    particle2->registerBuffer();
    particle2->setParticleStartColor({ 1.f, 1.f, 1.f, 1.f }, { 0.9f, 0.9f, 0.9f, 1.f });
    particle2->setIsLooping(false);
    particle2->addParticleModule(new ParticleIntervalSpawn(10));
    particle2->addParticleIndividualModule(new ScaleThroughParticleLifetime(0.1f, 1.f));
    particle2->setParticleLifetime(4, 6);
    particle2->setParticleStartVelocity(1, 1);
    particle2->setGravity({ 0, -0.6f });
    particle2->setParticleStartVelocityDirectionOverride([](vec2& velocity) {
        velocity[0] = randomFloat(-0.2f, 0.2f); // Override X direction
        velocity[1] = 1.f; // Override Y direction
    });

    // particle2->addParticleModule(new ParticleBurstSpawn(1.0f, 20));
    pNode5->addComponent(particle2);
    pNode5->addComponent(new TwoPointsMovement({ -0.5f, 0.f }, { 0.5f, 0.f }, 2.0f));
    pNode5->setActive(true);
    pWorldScene->addNode(pNode5);
    oSerializer << pNode5;


    auto pNode6 = new Node(0.5f, 0, 0);
    auto particle3 = new ParticleSystem(100);
    particle3->setShader(pParticleShader);
    particle3->registerBuffer();
    particle3->setParticleStartColor({ 1.f, 0.f, 0.f, 1.f }, { 0.9f, 0.f, 0.f, 1.f });
    particle3->setSpawnShape(eParticleSpawnShape::BOX);
    particle3->setSpawnShapeDimensions(0.2f, 0.5f);
    // particle3->addParticleModule(new ParticleIntervalSpawn(10));
    particle3->addParticleModule(new ParticleBurstSpawn(1.0f, 20));
    pNode6->addComponent(particle3);
    pNode6->setActive(true);
    pWorldScene->addNode(pNode6);

    oSerializer << pNode6;


    auto pNode7 = new Node(-0.5f, 0, 0);
    auto particle4 = new ParticleSystem(100);
    particle4->setShader(pParticleShader);
    particle4->registerBuffer();
    particle4->setSpawnShape(eParticleSpawnShape::CIRCLE);
    particle4->setSpawnShapeDimensions(0.4f, 0.4f);
    particle4->setParticleStartColor({ 0.f, 0.f, 0.9f, 1.f }, { 0.f, 0.f, 1.f, 1.f });
    particle4->addParticleModule(new ParticleIntervalSpawn(10));
    particle4->addParticleModule(new ParticleBurstSpawn(0.0f, 20));
    pNode7->addComponent(particle4);
    pNode7->setActive(true);
    pWorldScene->addNode(pNode7);

    oSerializer << pNode7;

    {
        Image* pCharacter = ImageLoader::getInstance()->getImageByPath("assets/images/character_animation.png");

        auto pPlayer = new Node(0.f, 0.f, 0.f);
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

        oSerializer << pPlayer;
    }

    oSerializer.finish();
}

void createVisualEffectDemo()
{
    WorldScene* const pWorldScene = WorldScene::current;

    {
        Skybox* pSkybox = new Skybox();
        pSkybox->loadSkyboxCubmaps(
            "assets/images/skybox/right.jpg",
            "assets/images/skybox/left.jpg",
            "assets/images/skybox/top.jpg",
            "assets/images/skybox/bottom.jpg",
            "assets/images/skybox/front.jpg",
            "assets/images/skybox/back.jpg"
        );
        pWorldScene->setSkybox(pSkybox);
    }

    Shader* p3DMeshShader = ShaderLoader::getInstance()->getShader("3d_mesh");

    // Node* pMonkey = loadModel("assets/models/monkey.obj", p3DMeshShader);
    // pMonkey->addComponent(new Rotate3D(10, 30.f, 20.f));
    // // pMonkey->addComponent(new TwoPointsMovement(vec2{-1, 0}, vec2{1, 0}, 2.0f));
    // pMonkey->addComponent(new TwoPointScaling(Vector3{0.5f, 0.5f, 0.5f}, Vector3{1.5f, 1.5f, 1.5f}, 2.0f));
    // addNode(pMonkey);

    {
        Image* const pMainImage = ImageLoader::getInstance()->getImageByPath("assets/images/container.png");
        Image* const pMetallicImage = ImageLoader::getInstance()->getImageByPath("assets/images/container_specular.png");

        std::shared_ptr<Material> pMaterial = std::make_shared<Material>(p3DMeshShader);
        pMaterial->setAlbedoMap(pMainImage);
        pMaterial->setSpecularMap(pMetallicImage);

        Node* pBackPackObj = loadModel("assets/models/box.obj", pMaterial);
        pBackPackObj->setPosition(-2.f, 0.f, 0.f);
        pWorldScene->addNode(pBackPackObj);
    }

    {
        Image* const pMainImage = ImageLoader::getInstance()->getImageByPath("assets/models/1001_albedo.jpg");
        Image* const pNormalImage = ImageLoader::getInstance()->getImageByPath("assets/models/1001_normal.png");
        Image* const pMetallicImage = ImageLoader::getInstance()->getImageByPath("assets/models/1001_metallic.jpg");

        std::shared_ptr<Material> pMaterial = std::make_shared<Material>(p3DMeshShader);
        pMaterial->setAlbedoMap(pMainImage);
        pMaterial->setNormalMap(pNormalImage);
        pMaterial->setSpecularMap(pMetallicImage);

        Node* pBackPackFbx = loadModel("assets/models/back_pack.fbx", pMaterial);
        pBackPackFbx->setScale(0.01f);
        pBackPackFbx->setPosition(2.f, 0.f, 0.f);
        pWorldScene->addNode(pBackPackFbx);
    }

    {
        Node* pPointLightNode = new Node();
        pPointLightNode->setRotationQuaternion(Quaternion::fromAxisAngle({1.f, 0.f, 0.f}, 45.f));

        DirectionLightComponent* pPointLightComp = new DirectionLightComponent();
        pPointLightComp->setColor({1.f, 1.f, 0.f});
        pPointLightComp->setIntensity(.4f);
        pPointLightNode->addComponent(pPointLightComp);

        pWorldScene->addNode(pPointLightNode);
    }

    {
        Node* pPointLightNode = new Node();
        pPointLightNode->setPosition(0.f, 3.f, 0.f);

        PointLightComponent* pPointLightComp = new PointLightComponent();
        pPointLightComp->setColor({1.f, 1.f, 0.f});
        pPointLightComp->setIntensity(1.f);
        pPointLightComp->setRange(1.f);
        pPointLightNode->addComponent(pPointLightComp);

        pPointLightNode->addComponent(new TwoPointsMovement(vec3{-6.f, 0, 2.f}, vec3{6.f, 0, 2.f}, 8.f));

        pWorldScene->addNode(pPointLightNode);
    }

    // {
    //     Node* pPointLightNode = new Node();

    //     PointLightComponent* pPointLightComp = new PointLightComponent();
    //     pPointLightComp->setColor({0.f, 1.f, 0.f});
    //     pPointLightComp->setIntensity(5.f);
    //     pPointLightComp->setRange(10.f);
    //     pPointLightNode->addComponent(pPointLightComp);

    //     pPointLightNode->addComponent(new TwoPointsMovement(vec2{3.f, 0.f}, vec2{3.f, 0.f}, 3.f));

    //     addNode(pPointLightNode);
    // }

    /*

    SimpleObjReader oSimpleObjectReader;
    std::shared_ptr<Mesh> readerMonkey = oSimpleObjectReader.loadWavefrontFile("assets/models/monkey_smooth.obj");
    std::shared_ptr<Mesh> readerBox = oSimpleObjectReader.loadWavefrontFile("assets/models/box.obj");

    {
        auto pNode = new Node(0.f, 0.f, 0.f, 0.f);
        pNode->addComponent(new Rotate3D(5, 10, 15));

        auto pMeshRenderer = new MeshRenderer();
        pMeshRenderer->setMesh(readerMonkey);
        pMeshRenderer->setShader(p3DMeshShader);
        pNode->addComponent(pMeshRenderer);

        // pNode->addComponent(new TwoPointsMovement(vec2{-2, 0}, vec2{1, 0}, 3.f));

        {
            auto pChildNode = new Node(2.5f, 0.f, 0.f, 0.f);

            auto pMeshRenderer = new MeshRenderer();
            pMeshRenderer->setMesh(readerBox);
            pMeshRenderer->setShader(p3DMeshShader);
            pChildNode->addComponent(pMeshRenderer);
            pChildNode->addComponent(new Rotate3D(-15, -25, -35));

            pNode->addChildNode(pChildNode);
        }

        addNode(pNode);
    }

    {
        auto pNode = new Node(1.f, 0.f, 0.f, 0.f);
        pNode->addComponent(new Rotate3D(15, 25, 35));

        auto pMeshRenderer = new MeshRenderer();
        pMeshRenderer->setMesh(readerBox);
        pMeshRenderer->setShader(p3DMeshShader);
        pMeshRenderer->setMainTexture(ImageLoader::getInstance()->getImageByPath("assets/images/container.png"));
        pMeshRenderer->setSpecularMap(ImageLoader::getInstance()->getImageByPath("assets/images/container_specular.png"));
        pNode->addComponent(pMeshRenderer);

        addNode(pNode);
    }
    */
}

void createLightingShadowDemo()
{
    WorldScene* const pWorldScene = WorldScene::current;

    {
        auto pCameraNode = new Node(0.f, 2.f, 6.f);
        pCameraNode->setRotationQuaternion(Quaternion::fromEulerAngles({-.5f, 0.f, 0.f}));
        pCameraNode->setName("Camera");
        Camera* pCamera = new Camera();
        pCamera->setUseOrthoProjection(false);
        pCamera->setWorldSizeScale(1.0f);
        pCameraNode->addComponent(pCamera);

        pWorldScene->addNode(pCameraNode);
    }

    {
        Skybox* pSkybox = new Skybox();
        pSkybox->loadSkyboxCubmaps(
            "assets/images/skybox/right.jpg",
            "assets/images/skybox/left.jpg",
            "assets/images/skybox/top.jpg",
            "assets/images/skybox/bottom.jpg",
            "assets/images/skybox/front.jpg",
            "assets/images/skybox/back.jpg"
        );
        pWorldScene->setSkybox(pSkybox);
    }

    Shader* p3DMeshShader = ShaderLoader::getInstance()->getShader("3d_mesh");

    {
        std::shared_ptr<Material> pMaterial = std::make_shared<Material>(p3DMeshShader);
        Node* pGround = loadModel("assets/models/box.obj", pMaterial);
        pGround->setPosition(0.f, -3.f, 0.f);
        pGround->setScale(10.f, 0.5f, 10.f);
        pWorldScene->addNode(pGround);
    }

    {
        Image* const pMainImage = ImageLoader::getInstance()->getImageByPath("assets/images/container.png");
        Image* const pMetallicImage = ImageLoader::getInstance()->getImageByPath("assets/images/container_specular.png");

        std::shared_ptr<Material> pMaterial = std::make_shared<Material>(p3DMeshShader);
        pMaterial->setAlbedoMap(pMainImage);
        pMaterial->setSpecularMap(pMetallicImage);

        Node* pBackPackObj = loadModel("assets/models/box.obj", pMaterial);
        pBackPackObj->setPosition(-2.f, 0.f, 0.f);
        pWorldScene->addNode(pBackPackObj);
    }

    {
        Image* const pMainImage = ImageLoader::getInstance()->getImageByPath("assets/models/1001_albedo.jpg");
        Image* const pNormalImage = ImageLoader::getInstance()->getImageByPath("assets/models/1001_normal.png");
        Image* const pMetallicImage = ImageLoader::getInstance()->getImageByPath("assets/models/1001_metallic.jpg");

        std::shared_ptr<Material> pMaterial = std::make_shared<Material>(p3DMeshShader);
        pMaterial->setAlbedoMap(pMainImage);
        pMaterial->setNormalMap(pNormalImage);
        pMaterial->setSpecularMap(pMetallicImage);

        Node* pBackPackFbx = loadModel("assets/models/back_pack.fbx", pMaterial);
        pBackPackFbx->setScale(0.01f);
        pBackPackFbx->setPosition(2.f, 0.f, 0.f);
        pWorldScene->addNode(pBackPackFbx);
    }

    {
        Node* pDirectionLightNode = new Node(0, 10.f, 0.f);
        pDirectionLightNode->setRotationQuaternion(Quaternion::fromEulerAngles({-45.f, 45.f, 0.f}));

        DirectionLightComponent* pPointLightComp = new DirectionLightComponent();
        pPointLightComp->setColor({1.f, 1.f, .5f});
        pPointLightComp->setIntensity(2.f);
        pDirectionLightNode->addComponent(pPointLightComp);

        pWorldScene->addNode(pDirectionLightNode);
    }
}
