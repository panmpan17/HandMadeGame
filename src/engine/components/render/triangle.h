#pragma once

#include "../drawable_interface.h"
#include "../../render/vertex.h"
#include "../../render/shader.h"

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

    void setShader(Shader* pShader) override;

private:
    Shader* m_pShader = nullptr;

    GLuint m_nVertexBuffer, m_nVertexArray;
    const ShaderUniformHandle* m_pMVPHandle = nullptr;

    COMPONENT_REGISTER_SERIALIZABLE(Triangle)
};

REGISTER_CLASS(Triangle)
