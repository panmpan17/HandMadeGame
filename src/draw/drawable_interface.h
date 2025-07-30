#pragma once

class Shader;

class IDrawable
{
public:
    virtual void registerBuffer() = 0;
    virtual void draw() = 0;
    virtual void setShader(Shader* pShader) = 0;
};

