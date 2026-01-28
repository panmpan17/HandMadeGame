#include "world.h"

#include <glad/gl.h>
#include "node.h"
#include "../camera.h"
#include "../window.h"
#include "../math/random.h"
#include "../serialization/serializer.h"
#include "../../render/skybox.h"
#include "../../render/renderer.h"
#include "../../components/input/first_person_free_control_camera.h"
#include "../../components/drawable_interface.h"
#include "../../../editor/gizmos.h"

WorldScene* WorldScene::current = nullptr;


WorldScene::WorldScene()
{
    current = this;

    m_oOpaqueDrawableArray.setCallDeleteOnDestruct(false);
    m_oTransparentDrawableArray.setCallDeleteOnDestruct(false);
}

WorldScene::~WorldScene()
{
}

void WorldScene::init()
{
    if (m_bInitEditorCamera)
    {
        auto pNode = new Node(0.f, 0.f, 2.5f);
        pNode->setName("Editor Camera");
        pNode->setShouldSerialize(false);

        auto pCameraNode = new Node(0.f, 0.f, 0.f);
        pCameraNode->setName("Camera");
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

    const std::vector<ISerializable*>& vecDeserializedObjects = deserializer.getDeserializedObjects();

    int nSize = vecDeserializedObjects.size();
    Node* pCurrentNode = nullptr;
    for (int i = 0; i < nSize; ++i)
    {
        ISerializable* pObject = vecDeserializedObjects[i];
        if (Node* pNode = dynamic_cast<Node*>(pObject))
        {
            if (pCurrentNode)
            {
                pCurrentNode->onFinishedDeserialization();
            }

            pCurrentNode = pNode;

            if (pNode->getParentNode() == nullptr)
            {
                addNode(pNode);
            }
        }
        else if (NodeComponent* pComponent = dynamic_cast<NodeComponent*>(pObject))
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

void WorldScene::update(float fDeltatime)
{
    int nSize = m_oNodeArray.getCount();
    for (int i = 0; i < nSize; ++i)
    {
        Node* pNode = m_oNodeArray.getElement(i);
        if (pNode && pNode->isActive() && !pNode->getIsStarted())
        {
            pNode->onStart();
        }
    }

    nSize = m_oNodeArray.getCount();
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
    bool bUsingOpenGL = Window::ins->isUsingOpenGL();
    bool bUsingMetal = Window::ins->isUsingMetal();

    if (bUsingOpenGL)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
#if __APPLE__
    else if (bUsingMetal)
    {
        MTL::RenderCommandEncoder* pRenderEncoder = Window::ins->getCurrentFrameRenderEncoder();
        pRenderEncoder->setDepthStencilState(Renderer::m_pDepthOnStencilState);
        pRenderEncoder->setCullMode(MTL::CullModeBack);
        pRenderEncoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
    }
#endif // __APPLE__

    int nSize = m_oOpaqueDrawableArray.getCount();
    for (int i = 0; i < nSize; ++i)
    {
        IDrawable* pDrawable = m_oOpaqueDrawableArray.getElement(i);
        if (pDrawable)
        {
            pDrawable->draw();
        }
    }

    if (m_pSkybox)
    {
        m_pSkybox->draw();
    }

    renderTransparentObjects();
}

void WorldScene::renderTransparentObjects()
{
    bool bUsingOpenGL = Window::ins->isUsingOpenGL();
    bool bUsingMetal = Window::ins->isUsingMetal();

    if (bUsingOpenGL)
    {
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
    }
#if __APPLE__
    else if (bUsingMetal)
    {
        MTL::RenderCommandEncoder* pRenderEncoder = Window::ins->getCurrentFrameRenderEncoder();
        pRenderEncoder->setDepthStencilState(Renderer::m_pDepthOffStencilState);

    }
#endif // __APPLE__

    int nTransparentSize = m_oTransparentDrawableArray.getCount();
    for (int i = 0; i < nTransparentSize; ++i)
    {
        IDrawable* pDrawable = m_oTransparentDrawableArray.getElement(i);
        if (pDrawable)
        {
            pDrawable->draw();
        }
    }

    if (bUsingOpenGL)
    {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
#if __APPLE__
    else if (bUsingMetal)
    {
        MTL::RenderCommandEncoder* pRenderEncoder = Window::ins->getCurrentFrameRenderEncoder();
        pRenderEncoder->setDepthStencilState(Renderer::m_pDepthOnStencilState);
    }
#endif // __APPLE__
}

void WorldScene::renderDepth()
{
    int nSize = m_oOpaqueDrawableArray.getCount();
    for (int i = 0; i < nSize; ++i)
    {
        IDrawable* pDrawable = m_oOpaqueDrawableArray.getElement(i);
        if (pDrawable)
        {
            pDrawable->drawDepth();
        }
    }
}

void WorldScene::drawGizmos()
{
    GizmosManager::getInstance()->clearAllGizmos();
    
    int nCount = m_oNodeArray.getCount();
    for (int i = 0; i < nCount; ++i)
    {
        Node* pNode = m_oNodeArray.getElement(i);
        if (pNode && pNode->isActive())
        {
            pNode->drawGizmos();
        }
    }

    GizmosManager::getInstance()->drawAllGizmos();
}

void WorldScene::addNode(Node* pNode)
{
    if (pNode == nullptr)
        return;
    m_oNodeArray.addElement(pNode);

    pNode->onAddToWorldScene();
}

void WorldScene::addDrawable(IDrawable* const pDrawable)
{
    if (pDrawable == nullptr)
        return;

    if (pDrawable->getIsTransparent())
    {
        m_oTransparentDrawableArray.addElement(pDrawable);
    }
    else
    {
        m_oOpaqueDrawableArray.addElement(pDrawable);
    }
}

void WorldScene::removeDrawable(IDrawable* const pDrawable)
{
    if (pDrawable == nullptr)
        return;

    if (pDrawable->getIsTransparent())
    {
        m_oTransparentDrawableArray.removeElement(pDrawable);
    }
    else
    {
        m_oOpaqueDrawableArray.removeElement(pDrawable);
    }
}
