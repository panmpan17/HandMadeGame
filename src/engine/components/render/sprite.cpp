#include "sprite.h"

#include <glad/gl.h>
#include "../../core/debug_macro.h"
#include "../../core/serialization/serializer.h"
#include "../../core/math/random.h"


Sprite::Sprite(Image* pImage, int nPixelPerUnit/* = 100*/)
    // : Quad(0.5f, 0.5f, {1.f, 1.f, 1.f, 1.f})
{
    m_nID = generateRandomUUID();

    m_pImage = pImage;
    m_fWidth = pImage->getWidth() * (1.f / nPixelPerUnit);
    m_fHeight = pImage->getHeight() * (1.f / nPixelPerUnit);
}

Sprite::Sprite(Image* pImage, int nSpriteSheetXCount, int nSpriteSheetYCount, int nSpriteIndex/* = 0 */, int nPixelPerUnit/* = 100*/)
{
    m_nID = generateRandomUUID();
    
    m_pImage = pImage;
    m_fWidth = (pImage->getWidth() / (float)nSpriteSheetXCount) * (1.f / nPixelPerUnit);
    m_fHeight = (pImage->getHeight() / (float)nSpriteSheetYCount) * (1.f / nPixelPerUnit);
    m_vecSpriteSheetCount.x = nSpriteSheetXCount;
    m_vecSpriteSheetCount.y = nSpriteSheetYCount;

    setSpriteIndex(nSpriteIndex);
}

void Sprite::serializeToWrapper(DataSerializer& serializer) const
{
    Quad::serializeToWrapper(serializer);
}

bool Sprite::deserializeField(DataDeserializer& deserializer, std::string_view strFieldName, std::string_view strFieldValue)
{
    return Quad::deserializeField(deserializer, strFieldName, strFieldValue);
}
