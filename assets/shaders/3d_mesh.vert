#version 330

layout(std140) uniform CameraMatrices
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_CamPos;
};

uniform mat4 u_Model;

layout (location = 0) in vec3 a_vPos;
layout (location = 1) in vec2 a_vUV;
layout (location = 2) in vec3 a_vNormal;

out vec2 fragUV;
out vec3 fragPos;
out vec3 fragNormal;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_vPos, 1.0);

    fragPos = vec3(u_Model * vec4(a_vPos, 1.0));
    fragUV = a_vUV;
    fragNormal = mat3(transpose(inverse(u_Model))) * a_vNormal;
}
