#version 330

uniform mat4 u_MVP;
uniform vec2 u_Size;

layout (location = 0)in vec2 a_vPos;
layout (location = 1)in vec2 a_vUV;

out vec2 uv;

void main()
{
    vec2 scaledPos = a_vPos * u_Size;
    gl_Position = u_MVP * vec4(scaledPos, 0.0, 1.0);
    uv = a_vUV;
    uv.y = 1 - uv.y;
}
