#version 330

uniform sampler2D u_tex0;

in vec2 uv;

out vec4 fragment;


int[16] bayerMatrix4x4 = int[16](
    0,  8,  2, 10,
    12,  4, 14,  6,
    3, 11,  1,  9,
    15,  7, 13,  5
);
// int[16] bayerMatrix4x4 = int[16](
//     0, 0, 0, 0,
//     0, 0, 0, 0,
//     0, 0, 0, 0,
//     0, 0, 0, 0
// );

int[64] bayerMatrix8x8 = int[64](
    10, 51, 2, 48, 40, 31, 53, 30,
    25, 35, 60, 5, 50, 34, 58, 7,
    32, 38, 49, 63, 55, 20, 33, 36,
    4, 61, 22, 28, 27, 52, 57, 44,
    42, 43, 54, 0, 45, 46, 14, 41,
    23, 19, 39, 21, 8, 29, 3, 16,
    6, 17, 47, 13, 15, 11, 56, 59,
    26, 24, 1, 62, 9, 12, 18, 37
);


vec4 ditherMethod4x4(vec4 textColor, int x, int y)
{
    int threshold = bayerMatrix4x4[(y % 4) * 4 + (x % 4)];
    float normalizedThreshold = float(threshold) / 16.0;

    vec3 ditheredColor = floor(textColor.rgb * 16.0 + normalizedThreshold) / 16.0;
    return vec4(ditheredColor, textColor.a);
}

vec4 ditherMethod8x8(vec4 textColor, int x, int y)
{
    int threshold = bayerMatrix8x8[(y % 8) * 8 + (x % 8)];
    float normalizedThreshold = float(threshold) / 64.0;

    vec3 ditheredColor = floor(textColor.rgb * 64.0 + normalizedThreshold) / 64.0;
    return vec4(ditheredColor, textColor.a);
}

void main()
{
    vec4 textColor = texture(u_tex0, uv);
    // vec4 textColor = vec4(uv.x, 0, 0.0, 1.0);

    int u_textureWidth = 1920 / 8;
    int u_textureHeight = 1080 / 8;

    int x = int(uv.x * u_textureWidth);
    int y = int(uv.y * u_textureHeight);

    fragment = ditherMethod4x4(textColor, x, y);
    // fragment = ditherMethod8x8(textColor, x, y);
}