#version 330

uniform mat4 u_MVP;
uniform mat4 u_nodeTransform;
uniform bool u_useNodeTransform;

layout (location = 0) in vec2 quadPos;
layout (location = 1) in vec2 quadTexCoord;
layout (location = 2) in vec2 instancePos;
layout (location = 3) in vec4 instanceColor;
layout (location = 4) in float rotation;
layout (location = 5) in float scale;

out vec4 fragmentColor;
out vec2 uv;

void main()
{
    mat2 rotationMatrix = mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation));
    vec2 scaledPos = quadPos * scale;

    if (u_useNodeTransform)
    {
        gl_Position = u_nodeTransform * vec4((rotationMatrix * scaledPos) + instancePos, 0.0, 1.0);
    }
    else
    {
        gl_Position = u_MVP * vec4((rotationMatrix * scaledPos) + instancePos, 0.0, 1.0);
    }

    fragmentColor = instanceColor;
    uv = quadTexCoord;
}
