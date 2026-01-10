#include "minimal_metal_test.h"

#include "../../engine/core/debug_macro.h"
#include "../../engine/core/scene/node.h"
#include "../../engine/core/scene/world.h"
#include "../../engine/components/render/triangle.h"
#include "../../engine/render/shader.h"
#include "../../engine/render/shader_loader.h"
#include "../../engine/components/transform/rotate.h"
#include "../../engine/components/transform/movement.h"


void firstTriangeTest()
{
    WorldScene* const pWorldScene = WorldScene::current;

    { // Triangle
        Shader* const pColorShared = ShaderLoader::getInstance()->getShader("colored_vertices");
        LOGLN("Shader loaded: {}", (void*)pColorShared);

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