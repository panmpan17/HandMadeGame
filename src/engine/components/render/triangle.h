#pragma once

#include "../drawable_interface.h"
#include "../../render/vertex.h"
#include "../../render/shader.h"

typedef unsigned int GLuint;


namespace MTL { class Buffer; }


class Triangle : public IDrawable
{
public:

    Triangle();
    ~Triangle();

    void registerBuffer();
    void draw() override;

    void setShader(Shader* pShader) override;

private:
    Shader* m_pShader = nullptr;

    MTL::Buffer* m_pPosBuffer = nullptr;
    MTL::Buffer* m_pColBuffer = nullptr;

    GLuint m_nVertexBuffer, m_nVertexArray;
    const ShaderUniformHandle* m_pMVPHandle = nullptr;

    COMPONENT_REGISTER_SERIALIZABLE(Triangle)
public:
    virtual void onNodeFinishedDeserialization() override;
};

REGISTER_CLASS(Triangle)
