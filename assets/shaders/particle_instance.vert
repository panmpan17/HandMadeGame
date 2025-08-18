#version 330

uniform mat4 MVP;

layout (location = 0) in vec2 quadPos;
layout (location = 1) in vec2 instancePos;
layout (location = 2) in vec4 instanceColor;

out vec4 fragmentColor;

void main()
{
    gl_Position = MVP * vec4(quadPos + instancePos, 0.0, 1.0);
    fragmentColor = instanceColor;
}
