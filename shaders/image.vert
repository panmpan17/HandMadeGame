#version 330
uniform mat4 MVP;
uniform vec3 color;

in vec2 vPos;

void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    color = vec4(1, 1, 1, 1);
}
