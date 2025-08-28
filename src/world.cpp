#include "world.h"
#include "node.h"
#include "image.h"
#include "draw/shader.h"
#include "draw/quad.h"
#include "draw/triangle.h"
#include "components/rotate.h"
#include "components/movement.h"
#include "components/sprite_animation.h"
#include "components/character2d.h"
#include "components/particle/simple_particle_system.h"
#include "components/particle/particle_system.h"
#include "components/particle/particle_spawn.h"
#include "components/particle/particle_lifetime_change.h"
#include "random.h"
#include "serializer.h"


WorldScene::WorldScene()
{

}

WorldScene::~WorldScene()
{
    if (m_pBaseShader)
    {
        delete m_pBaseShader;
        m_pBaseShader = nullptr;
    }
    if (m_pImageShader)
    {
        delete m_pImageShader;
        m_pImageShader = nullptr;
    }
}

void WorldScene::init()
{   
    // TODO: remove this
    DataSerializer oSerializer("world.txt");

    m_pBaseShader = new TestShader();
    m_pImageShader = new ImageShader();

    Image* pTestImage = ImageLoader::getInstance()->getImage("test");
    Image* pDustImage = ImageLoader::getInstance()->getImage("dust");

    { // Triangle
        auto pNode = new Node(-0.5f, 0.f, 0.f, 0.f);

        auto pTriangle = new Triangle();
        pTriangle->setShader(m_pBaseShader);
        pTriangle->registerBuffer();
        pNode->addComponent(pTriangle);

        pNode->addComponent(new Rotate(-1.0f));

        pNode->addComponent(new Movement(1.0f)); // Add movement component with speed 1.0f

        addNode(pNode);

        oSerializer << pNode;
    }

    {
        auto pNode = new Node(-0.4f, 0.f, 0.f, 0.f);

        auto pTriangle = new Triangle();
        pTriangle->setShader(m_pBaseShader);
        pTriangle->registerBuffer();
        pNode->addComponent(pTriangle);

        addNode(pNode);

        oSerializer << pNode;
    }

    // Quads 1
    auto pNode2 = new Node(0.5f, 0.5f, 0.f, 0.f);

    vec4 red = {1.f, 0.f, 0.f, 1.f}; // Red color for the quad
    auto pQuad = new Quad(0.5f, 0.5f, red);
    pQuad->setShader(m_pImageShader);
    static_cast<Quad*>(pQuad)->setImage(pTestImage);
    pQuad->registerBuffer();
    pNode2->addComponent(pQuad);

    pNode2->addComponent(new Rotate(1.0f));

    addNode(pNode2);

    oSerializer << pNode2;

    // Quads 2
    auto pNode3 = new Node(0.5f, 0.5f, 0.f, 0.f);

    vec4 color = {0.5f, 0.5f, 1.f, 1.f}; // Blue color for the second quad
    auto pQuad2 = new Quad(0.3f, 0.3f, color);
    pQuad2->setShader(m_pImageShader);
    static_cast<Quad*>(pQuad2)->setImage(pDustImage);
    pQuad2->registerBuffer();
    pNode3->addComponent(pQuad2);

    addNode(pNode3);

    oSerializer << pNode3;

    // Particle System
    // auto pNode4 = new Node(0, 0, 0, 0);
    // auto particle = new SimpleParticleSystem(10000);
    // particle->setShader(new ParticleShader());
    // particle->registerBuffer();
    // pNode4->addComponent(particle);
    // addNode(pNode4);

    auto pNode5 = new Node(0, 0, 0, 0);
    auto particle2 = new ParticleSystem(100, false);
    particle2->setImage(pDustImage);
    particle2->setShader(new ParticleInstanceShader());
    particle2->registerBuffer();
    particle2->setParticleStartColor({ 1.f, 0.f, 0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f });
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
    pNode5->setActive(false);
    addNode(pNode5);
    oSerializer << pNode5;


    auto pNode6 = new Node(0.5f, 0, 0, 0);
    auto particle3 = new ParticleSystem(100);
    particle3->setShader(new ParticleInstanceShader());
    particle3->registerBuffer();
    particle3->setParticleStartColor({ 0.f, 0.f, 1.f, 1.f }, { 0.f, 1.f, 0.f, 1.f });
    particle3->setSpawnShape(eParticleSpawnShape::BOX);
    particle3->setSpawnShapeDimensions(0.2f, 0.5f);
    // particle3->addParticleModule(new ParticleIntervalSpawn(10));
    particle3->addParticleModule(new ParticleBurstSpawn(1.0f, 20));
    pNode6->addComponent(particle3);
    pNode6->setActive(false);
    addNode(pNode6);

    oSerializer << pNode6;


    auto pNode7 = new Node(-0.5f, 0, 0, 0);
    auto particle4 = new ParticleSystem(100);
    particle4->setShader(new ParticleInstanceShader());
    particle4->registerBuffer();
    particle4->setSpawnShape(eParticleSpawnShape::CIRCLE);
    particle4->setSpawnShapeDimensions(0.4f, 0.4f);
    particle4->setParticleStartColor({ 1.f, 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f, 1.f });
    particle4->addParticleModule(new ParticleIntervalSpawn(10));
    particle4->addParticleModule(new ParticleBurstSpawn(0.0f, 20));
    pNode7->addComponent(particle4);
    pNode7->setActive(false);
    addNode(pNode7);

    oSerializer << pNode7;

    {
        Image* pCharacter = ImageLoader::getInstance()->getImage("character");

        auto pPlayer = new Node(0.f, 0.f, 0.f, 0.f);
        auto pSprite = new Sprite(pCharacter, 4, 4);
        pSprite->setShader(m_pImageShader);
        pSprite->registerBuffer();
        pPlayer->addComponent(pSprite);

        auto pSpriteAnimation = new SpriteAnimation(pSprite);
        pPlayer->addComponent(pSpriteAnimation);

        auto pCharacter2d = new Character2d(pSpriteAnimation);
        pPlayer->addComponent(pCharacter2d);

        addNode(pPlayer);

        oSerializer << pPlayer;
    }

    oSerializer.finish();
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
}

void WorldScene::addNode(Node* pNode)
{
    if (pNode == nullptr)
        return;
    m_oNodeArray.addElement(pNode);
}
