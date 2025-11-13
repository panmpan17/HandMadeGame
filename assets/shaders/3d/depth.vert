#version 330

layout (location = 0) in vec3 a_vPos;

uniform mat4 u_Model;
uniform mat4 u_LightMatrix;

void main()
{
    gl_Position = u_LightMatrix * u_Model * vec4(a_vPos, 1.0);
}
