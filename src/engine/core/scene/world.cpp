#include "world.h"
#include "node.h"
#include "../camera.h"
#include "../math/random.h"
#include "../serialization/serializer.h"
#include "../../render/skybox.h"
#include "../../render/image.h"
#include "../../render/shader_loader.h"
#include "../../components/input/first_person_free_control_camera.h"

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

        auto pCameraNode = new Node(0.f, 0.f, 0.f);
        Camera* pCamera = new Camera();
        pCamera->useAsMain();
        pCamera->setUseOrthoProjection(false);
        pCamera->setWorldSizeScale(1.0f);
        pCameraNode->addComponent(pCamera);
        pNode->addChildNode(pCameraNode);

        pNode->addComponent(new FirstPersonFreeControlCamera(pCameraNode));

        addNode(pNode);
    }
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

    if (m_pSkybox)
    {
        m_pSkybox->draw();
    }
}

void WorldScene::addNode(Node* pNode)
{
    if (pNode == nullptr)
        return;
    m_oNodeArray.addElement(pNode);
}
