#include "text_renderer.h"

TextRenderer::TextRenderer()
{
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::draw()
{}

void TextRenderer::setMaterial(const std::shared_ptr<Material>& pMaterial)
{
    m_pMaterial = pMaterial;
}
