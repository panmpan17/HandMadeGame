#pragma once

#include "../drawable_interface.h"
#include "../../render/vertex.h"
#include "../../render/shader.h"

typedef unsigned int GLuint;


#if __APPLE__
#include <Metal/Metal.hpp>
#endif // __APPLE__


class Triangle : public IDrawable
{
public:

    Triangle();
    ~Triangle();

    void registerBuffer();
    void draw() override;

    void setShader(Shader* pShader) override;

    virtual void onDrawGizmos(bool bIsSelected) override;

private:
    Shader* m_pShader = nullptr;

#if __APPLE__
    MTL::Buffer* m_pPosBuffer = nullptr;
    MTL::Buffer* m_pColBuffer = nullptr;
#endif // __APPLE__

    GLuint m_nVertexBuffer, m_nVertexArray;
    const ShaderUniformHandle* m_pMVPHandle = nullptr;

    COMPONENT_REGISTER_SERIALIZABLE(Triangle)
public:
    virtual void onNodeFinishedDeserialization() override;
};

REGISTER_CLASS(Triangle)
