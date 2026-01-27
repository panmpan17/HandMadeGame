#include <metal_stdlib>
#include "common.metal"

using namespace metal;

constant int bayerMatrix4x4[16] = {
    0,  8,  2, 10,
    12, 4, 14, 6,
    3, 11, 1,  9,
    15, 7, 13, 5
};

// constant int bayerMatrix8x8[64] = {
//     10, 51, 2, 48, 40, 31, 53, 30,
//     25, 35, 60, 5, 50, 34, 58, 7,
//     32, 38, 49, 63, 55, 20, 33, 36,
//     4, 61, 22, 28, 27, 52, 57, 44,
//     42, 43, 54, 0, 45, 46, 14, 41,
//     23, 19, 39, 21, 8, 29, 3, 16,
//     6, 17, 47, 13, 15, 11, 56, 59,
//     26, 24, 1, 62, 9, 12, 18, 37
// };

inline float4 ditherMethod4x4(float4 texColor, int x, int y)
{
    int threshold = bayerMatrix4x4[(y & 3) * 4 + (x & 3)];
    float normalizedThreshold = float(threshold) / 16.0f;

    float3 ditheredColor = floor(texColor.rgb * 16.0f + normalizedThreshold) / 16.0f;
    return float4(ditheredColor, texColor.a);
}

// inline float4 ditherMethod8x8(float4 texColor, int x, int y)
// {
//     int threshold = bayerMatrix8x8[(y & 7) * 8 + (x & 7)];
//     float normalizedThreshold = float(threshold) / 64.0f;

//     float3 ditheredColor = floor(texColor.rgb * 64.0f + normalizedThreshold) / 64.0f;
//     return float4(ditheredColor, texColor.a);
// }

fragment float4 orderDithering_fragmentMain(VertexOut_Image in [[stage_in]],
                                            metal::texture2d<float> texture0 [[texture(0)]],
                                            metal::sampler textureSampler [[sampler(0)]])
{
    // GLSL version flips v so keep the same behavior here.
    in.texCoord.y = 1.0 - in.texCoord.y;

    float4 texColor = texture0.sample(textureSampler, in.texCoord);

    // Match the original shader's use of a downscaled texture grid (1920/8, 1080/8).
    int textureWidth = max(1, int(texture0.get_width()) / 8);
    int textureHeight = max(1, int(texture0.get_height()) / 8);

    int x = int(in.texCoord.x * float(textureWidth));
    int y = int(in.texCoord.y * float(textureHeight));

    return ditherMethod4x4(texColor, x, y);
    // return ditherMethod8x8(texColor, x, y);
}