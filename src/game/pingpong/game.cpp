#include "game.h"

// #include "../../engine/core/serialization/serializer.h"
#include "paddle_control.h"
#include "pong.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/core/math/random.h"
#include "../../engine/render/image.h"
#include "../../engine/render/shader.h"
#include "../../engine/render/shader_loader.h"
#include "../../engine/components/render/quad.h"
#include "../../engine/components/render/triangle.h"
#include "../../engine/components/render/character2d.h"
#include "../../engine/components/transform/rotate.h"
#include "../../engine/components/transform/movement.h"
#include "../../engine/components/particle/particle_system.h"
#include "../../engine/components/particle/particle_spawn.h"
#include "../../engine/components/particle/particle_lifetime_change.h"

void createPingPongGame()
{
    WorldScene* const pWorldScene = WorldScene::current;

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

        pWorldScene->addNode(pPaddleLeft);
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

        pWorldScene->addNode(pPaddleRight);
    }

    {// Center line
        auto pCenterLine = new Node();
        pCenterLine->setPosition(0.f, 0.f);

        auto pQuad = new Quad(.1f, 5.625f, vecGray);
        pQuad->setShader(pImageShader);
        pQuad->registerBuffer();
        pCenterLine->addComponent(pQuad);

        pWorldScene->addNode(pCenterLine);
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

        pWorldScene->addNode(pBall);
    }
}