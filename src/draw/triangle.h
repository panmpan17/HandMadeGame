#pragma once

#include "vertex.h"
#include "shader.h"
#include "drawable_interface.h"

typedef unsigned int GLuint;



// REGISTER_CLASS(Triangle)
// inline std::string Triangle_getTypeName() { return "Triangle"; }
// namespace
// { struct Triangle_Registrator
//     { Triangle_Registrator()
//     { TypeRegistry::instance().registerType(Triangle_getTypeName(), []() -> ISerializable*
//     { return new Triangle(); }); } };
//     static Triangle_Registrator global_Triangle_registrator; }

// class Triangle : public IDrawable
// {

// };

class Triangle : public IDrawable
{
public:

    Triangle();
    ~Triangle();

    void registerBuffer() override;
    void draw() override;

    inline void setShader(Shader* pShader) override { m_pShader = pShader; m_nMVPUniform = m_pShader->getUniformLocation("u_MVP"); }

private:
    Shader* m_pShader = nullptr;

    GLuint m_nVertexBuffer, m_nVertexArray, m_nMVPUniform;

    COMPONENT_REGISTER_SERIALIZABLE(Triangle)
};

REGISTER_CLASS(Triangle)
