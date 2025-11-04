#include "world.h"
#include "node.h"
#include "../camera.h"
#include "../math/random.h"
#include "../serialization/serializer.h"
#include "../../render/skybox.h"
#include "../../render/image.h"
#include "../../render/shader.h"
#include "../../render/shader_loader.h"
#include "../../render/material.h"
#include "../../render/models/simple_obj_reader.h"
#include "../../render/models/assimp_model_reader.h"
#include "../../render/lighting/point_light.h"
#include "../../render/lighting/direction_light.h"
#include "../../components/render/quad.h"
#include "../../components/render/triangle.h"
#include "../../components/render/sprite_animation.h"
#include "../../components/render/character2d.h"
#include "../../components/render/mesh_renderer.h"
#include "../../components/transform/rotate.h"
#include "../../components/transform/movement.h"
#include "../../components/transform/scaling.h"
#include "../../components/particle/simple_particle_system.h"
#include "../../components/particle/particle_system.h"
#include "../../components/particle/particle_spawn.h"
#include "../../components/particle/particle_lifetime_change.h"
#include "../../components/input/first_person_free_control_camera.h"
#include "../../physics/box.h"

#include "../../../game/pingpong/paddle_control.h"
#include "../../../game/pingpong/pong.h"

WorldScene* WorldScene::current = nullptr;


WorldScene::WorldScene()
{
    current = this;
}

WorldScene::~WorldScene()
{
}

void WorldScene::init()
{
    {
        auto pNode = new Node(0.f, 0.f, 2.5f);
        pNode->addComponent(new FirstPersonFreeControlCamera());

        Camera* pCamera = new Camera();
        pCamera->useAsMain();
        pCamera->setUseOrthoProjection(false);
        pCamera->setWorldSizeScale(1.0f);
        pNode->addComponent(pCamera);

        addNode(pNode);
    }

    // TODO: remove this
    DataSerializer oSerializer("assets/level.txt");

    Shader* pTestShader = ShaderLoader::getInstance()->getShader("test");
    Shader* pImageShader = ShaderLoader::getInstance()->getShader("image");
    Shader* pParticleShader = ShaderLoader::getInstance()->getShader("particle_instance");

    Image* pTestImage = ImageLoader::getInstance()->getImage("test");
    Image* pDustImage = ImageLoader::getInstance()->getImage("dust");

    { // Triangle
        auto pNode = new Node(-0.5f, 0.f, 0.f);

        auto pTriangle = new Triangle();
        pTriangle->setShader(pTestShader);
        pTriangle->registerBuffer();
        pNode->addComponent(pTriangle);

        pNode->addComponent(new Rotate3D(0, 0, -10.0f));

        pNode->addComponent(new Movement(1.0f)); // Add movement component with speed 1.0f

        addNode(pNode);

        oSerializer << pNode;
    }

    {
        auto pNode = new Node(-0.4f, 0.f, 0.f);

        auto pTriangle = new Triangle();
        pTriangle->setShader(pTestShader);
        pTriangle->registerBuffer();
        pNode->addComponent(pTriangle);

        addNode(pNode);

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

    addNode(pNode2);

    oSerializer << pNode2;

    // Quads 2
    auto pNode3 = new Node(0.5f, 0.5f, 0.f);

    vec4 color = {0.5f, 0.5f, 1.f, 1.f}; // Blue color for the second quad
    auto pQuad2 = new Quad(0.3f, 0.3f, color);
    pQuad2->setShader(pImageShader);
    pQuad2->setImage(pDustImage);
    pQuad2->registerBuffer();
    pNode3->addComponent(pQuad2);

    addNode(pNode3);

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
    addNode(pNode5);
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
    addNode(pNode6);

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
    addNode(pNode7);

    oSerializer << pNode7;

    {
        Image* pCharacter = ImageLoader::getInstance()->getImage("character");

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

        addNode(pPlayer);

        oSerializer << pPlayer;
    }

    oSerializer.finish();
}

void WorldScene::createPinPongGame()
{
    {
        auto pNode = new Node(0.f, 0.f, 2.5f);
        pNode->addComponent(new FirstPersonFreeControlCamera());

        Camera* pCamera = new Camera();
        pCamera->useAsMain();
        pCamera->setUseOrthoProjection(false);
        pCamera->setWorldSizeScale(5.0f);
        pNode->addComponent(pCamera);

        addNode(pNode);
    }

    Shader* pImageShader = ShaderLoader::getInstance()->getShader("image");
    Shader* pParticleShader = ShaderLoader::getInstance()->getShader("particle_instance");

    vec4 vecGray = {.5f, .5f, .5f, 1.f};

    PaddleControl* pPaddleControlLeft;
    PaddleControl* pPaddleControlRight;
    

    auto pBall = new Node();

    {// Player 1 paddle (left side)
        auto pPaddleLeft = new Node();
        pPaddleLeft->setPosition(-4.5f, 0.f);

        Box oBox = Box::fromCenter(0, 0, 0.3f, 1.f);

        pPaddleControlLeft = new PaddleControl(oBox, PaddleControlType::PLAYER1, 2.5f);
        pPaddleControlLeft->setPositionBounds(-2.3f, 2.3f);
        pPaddleLeft->addComponent(pPaddleControlLeft);
        pPaddleControlLeft->setPong(pBall);
        pPaddleControlLeft->start();

        addNode(pPaddleLeft);
    }

    {// Player 2 paddle (right side)
        auto pPaddleRight = new Node();
        pPaddleRight->setPosition(4.5f, 0.f);

        Box oBox = Box::fromCenter(0, 0, 0.3f, 1.f);

        pPaddleControlRight = new PaddleControl(oBox, PaddleControlType::AI, 2.5f);
        pPaddleControlRight->setPositionBounds(-2.3f, 2.3f);
        pPaddleRight->addComponent(pPaddleControlRight);
        pPaddleControlRight->setPong(pBall);
        pPaddleControlRight->start();

        addNode(pPaddleRight);
    }

    {// Center line
        auto pCenterLine = new Node();
        pCenterLine->setPosition(0.f, 0.f);

        auto pQuad = new Quad(.1f, 5.625f, vecGray);
        pQuad->setShader(pImageShader);
        pQuad->registerBuffer();
        pCenterLine->addComponent(pQuad);

        addNode(pCenterLine);
    }

    {// Ball
        pBall->setPosition(0.f, 0.f);

        Box oBox = Box::fromCenter(0, 0, 0.6f, 0.6f);

        vec4 vecTransparentWhite = {1.f, 1.f, 1.f, 0.5f};

        auto particle = new ParticleSystem(10);
        // particle->setImage(nullptr);
        particle->setShader(pParticleShader);
        particle->registerBuffer();
        particle->setParticleStartScale(0.6f, 0.6f);
        particle->setParticleStartColor(vecTransparentWhite, vecTransparentWhite);
        particle->setIsLooping(true);
        particle->addParticleModule(new ParticleIntervalSpawn(10));
        particle->setParticleLifetime(0.5, 0.5);
        particle->setParticleStartVelocity(0, 0);
        particle->setGravity({ 0, 0 });
        particle->addParticleIndividualModule(new OpacityThroughParticleLifetime(1.f, 0.f));
        pBall->addComponent(particle);

        auto pPong = new Pong(oBox, pPaddleControlLeft, pPaddleControlRight, 4, 8);
        pPong->setPositionBounds(-2.8125, 2.8125);
        pBall->addComponent(pPong);
        pPong->start();

        addNode(pBall);
    }
}

void WorldScene::bloomTest()
{
    {
        auto pNode = new Node(0.f, 0.f, 2.5f);
        pNode->addComponent(new FirstPersonFreeControlCamera());

        Camera* pCamera = new Camera();
        pCamera->useAsMain();
        pCamera->setUseOrthoProjection(false);
        pCamera->setWorldSizeScale(10.0f);
        pNode->addComponent(pCamera);

        addNode(pNode);
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
        addNode(pBackPackObj);
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
        addNode(pBackPackFbx);
    }

    {
        Node* pPointLightNode = new Node();
        pPointLightNode->setRotationQuaternion(Quaternion::fromAxisAngle({1.f, 0.f, 0.f}, 45.f));

        DirectionLightComponent* pPointLightComp = new DirectionLightComponent();
        pPointLightComp->setColor({1.f, 1.f, 0.f});
        pPointLightComp->setIntensity(.4f);
        pPointLightNode->addComponent(pPointLightComp);

        addNode(pPointLightNode);
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

        addNode(pPointLightNode);
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
        pMeshRenderer->setMainTexture(ImageLoader::getInstance()->getImage("container"));
        pMeshRenderer->setSpecularMap(ImageLoader::getInstance()->getImage("container_specular"));
        pNode->addComponent(pMeshRenderer);

        addNode(pNode);
    }
    */
}


void WorldScene::readFromFiles(const std::string_view& strFilePath)
{
    Camera::main->setWorldSizeScale(1.0f);

    DataDeserializer deserializer(strFilePath);
    deserializer.read();

    std::vector<ISerializable*> deserializedObjects = deserializer.getDeserializedObjects();

    Shader* pTestShader = ShaderLoader::getInstance()->getShader("test");
    Shader* pImageShader = ShaderLoader::getInstance()->getShader("image");
    Shader* pParticleShader = ShaderLoader::getInstance()->getShader("particle_instance");

    Image* pTestImage = ImageLoader::getInstance()->getImage("test");

    int nSize = deserializedObjects.size();
    Node* pCurrentNode = nullptr;
    for (int i = 0; i < nSize; ++i)
    {
        ISerializable* pObject = deserializedObjects[i];
        if (Node* pNode = dynamic_cast<Node*>(pObject))
        {
            if (pCurrentNode)
            {
                pCurrentNode->onFinishedDeserialization();
            }

            pCurrentNode = pNode;
            addNode(pNode);
        }
        else if (Component* pComponent = dynamic_cast<Component*>(pObject))
        {
            pCurrentNode->addComponent(pComponent);
        }
        else
        {
            // Unknown object type, handle accordingly
            delete pObject; // Prevent memory leak
        }
    }

    if (pCurrentNode)
    {
        pCurrentNode->onFinishedDeserialization();
    }
}

void WorldScene::clearAllNodes()
{
    m_oNodeArray.clear();
}

void WorldScene::onStart()
{
    m_pSkybox = new Skybox();
    m_pSkybox->loadSkyboxCubmaps(
        "assets/images/skybox/right.jpg",
        "assets/images/skybox/left.jpg",
        "assets/images/skybox/top.jpg",
        "assets/images/skybox/bottom.jpg",
        "assets/images/skybox/front.jpg",
        "assets/images/skybox/back.jpg"
    );

    int nSize = m_oNodeArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Node* pNode = m_oNodeArray.getElement(i);
        if (pNode && pNode->isActive())
        {
            pNode->onStart();
        }
    }
}

void WorldScene::update(float fDeltatime)
{
    int nSize = m_oNodeArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Node* pNode = m_oNodeArray.getElement(i);
        if (pNode && pNode->isActive())
        {
            pNode->update(fDeltatime);
        }
    }
}

void WorldScene::render()
{
    int nSize = m_oNodeArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        Node* pNode = m_oNodeArray.getElement(i);
        if (pNode && pNode->isActive())
        {
            pNode->draw();
        }
    }

    m_pSkybox->draw();
}

void WorldScene::addNode(Node* pNode)
{
    if (pNode == nullptr)
        return;
    m_oNodeArray.addElement(pNode);
}
