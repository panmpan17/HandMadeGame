#version 330

#include "assets/shaders/base/camera_data.glsl"

uniform mat4 u_modelMatrix;
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
    mat2 rotationMatrix = mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation));
    vec2 scaledPos = quadPos * scale;

    if (u_useNodeTransform)
    {
        mat4 mvp = u_Projection * u_View * u_modelMatrix;
        gl_Position = mvp * vec4((rotationMatrix * scaledPos) + instancePos.xy, instancePos.z, 1.0);
    }
    else
    {
        mat4 vp = u_Projection * u_View;
        gl_Position = vp * vec4((rotationMatrix * scaledPos) + instancePos.xy, instancePos.z, 1.0);
    }

    fragmentColor = instanceColor * vec4(1.0, 1.0, 1.0, opacity);
    uv = quadTexCoord;
}
