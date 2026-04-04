#version 330

uniform sampler2D u_tex0;
uniform float u_threshold;

in vec2 uv;
uniform sampler2D u_LookupTexture; // The lookup table texture
uniform float u_LookupSize; // Size of one dimension of the lookup table (e.g., 16 for a 16x16x16 LUT)

out vec4 fragment;

vec4 lookup(vec4 color, float size)
{
     float r = color.r * (size - 1.0);
    float g = color.g * (size - 1.0);
    float b = color.b * (size - 1.0);

    float z = floor(b);
    float x = mod(z, size);
    float y = floor(z / size);

    vec2 lutUV = vec2(
        (r + x * size + 0.5) / (size * size),
        (g + y * size + 0.5) / (size)
    );

    return vec4(texture(u_LookupTexture, lutUV).xyz, color.a);
}

void main()
{
    vec4 textColor = texture(u_tex0, uv);
    fragment = lookup(textColor, u_LookupSize);
}
