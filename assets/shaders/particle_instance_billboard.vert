#version 330

#include "assets/shaders/base/camera_data.glsl"

uniform mat4 u_MVP;
uniform mat4 u_nodeTransform;
uniform bool u_useNodeTransform;

layout (location = 0) in vec2 quadPos;
layout (location = 1) in vec2 quadTexCoord;
layout (location = 2) in vec3 instancePos;
layout (location = 3) in vec4 instanceColor;
layout (location = 4) in float rotation;
layout (location = 5) in float scale;
layout (location = 6) in float opacity;

out vec4 fragmentColor;
out vec2 uv;

void main()
{
    mat3 viewRotation = mat3(u_View);
    mat3 billboardRotation = transpose(viewRotation);

    mat2 rotationMatrix = mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation));
    vec2 scaledPos = quadPos * scale;

    if (u_useNodeTransform)
    {
        // TODO: position is bit broken here
        gl_Position = u_nodeTransform * vec4(instancePos + billboardRotation * vec3(rotationMatrix * scaledPos, 0.0), 1.0);
    }
    else
    {
        gl_Position = u_Projection * u_View * vec4(instancePos + billboardRotation * vec3(rotationMatrix * scaledPos, 0.0), 1.0);
    }

    fragmentColor = instanceColor * vec4(1.0, 1.0, 1.0, opacity);
    uv = quadTexCoord;
}
