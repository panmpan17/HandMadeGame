#version 330

uniform sampler2D u_tex0;
uniform int u_textureWidth;

in vec2 uv;

out vec4 fragment;

void main()
{
    vec2 texelSize = vec2(1.0 / u_textureWidth, 0.0);
    vec4 sum = vec4(0.0);

    sum += texture(u_tex0, uv - 4.0 * texelSize) * 0.05;
    sum += texture(u_tex0, uv - 3.0 * texelSize) * 0.09;
    sum += texture(u_tex0, uv - 2.0 * texelSize) * 0.12;
    sum += texture(u_tex0, uv - 1.0 * texelSize) * 0.15;
    sum += texture(u_tex0, uv) * 0.18;
    sum += texture(u_tex0, uv + 1.0 * texelSize) * 0.15;
    sum += texture(u_tex0, uv + 2.0 * texelSize) * 0.12;
    sum += texture(u_tex0, uv + 3.0 * texelSize) * 0.09;
    sum += texture(u_tex0, uv + 4.0 * texelSize) * 0.05;

    fragment = sum;
}