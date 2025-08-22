#version 330

uniform mat4 MVP;
uniform mat4 NodeTransform;
uniform bool UseNodeTransform;

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

    if (UseNodeTransform)
    {
        gl_Position = NodeTransform * vec4((rotationMatrix * scaledPos) + instancePos, 0.0, 1.0);
    }
    else
    {
        gl_Position = MVP * vec4((rotationMatrix * scaledPos) + instancePos, 0.0, 1.0);
    }

    fragmentColor = instanceColor;
    uv = quadTexCoord;
}
