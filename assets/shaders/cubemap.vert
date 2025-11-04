#version 330 core
layout(location = 0) in vec3 aPos;

layout(std140) uniform CameraMatrices
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_CamPos;
};

out vec3 TexCoords;



void main()
{
    TexCoords = aPos;
    mat4 view = mat4(mat3(u_View)); // remove translation from the view matrix
    gl_Position = u_Projection * view * vec4(aPos, 1.0);
}