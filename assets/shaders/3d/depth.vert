#version 330

layout (location = 0) in vec3 a_vPos;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * vec4(a_vPos, 1.0);
}
