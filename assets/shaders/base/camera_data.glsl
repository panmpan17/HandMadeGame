layout(std140) uniform CameraMatrices
{
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_CamPos;
};