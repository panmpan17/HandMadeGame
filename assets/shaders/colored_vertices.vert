#version 330
uniform mat4 u_MVP;

in vec2 a_vPos;
in vec3 a_vCol;
out vec3 color;

void main()
{
    gl_Position = u_MVP * vec4(a_vPos, 0.0, 1.0);
    color = a_vCol;
}
