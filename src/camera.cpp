#include "camera.h"
// #include <GLFW/glfw3.h>

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::getViewMatrix(mat4x4& oOutViewMatrix)
{
    mat4x4 m, p;

    mat4x4_identity(m);
    // mat4x4_rotate_Z(m, m, (float) glfwGetTime());

    const float LEFT_CLIPPING_PLANE = -m_fRatio;
    const float RIGHT_CLIPPING_PLANE = m_fRatio;
    constexpr float BOTTOM_CLIPPING_PLANE = -1.f;
    constexpr float TOP_CLIPPING_PLANE = 1.f;
    constexpr float NEAR_CLIPPING_PLANE = 1.f;
    constexpr float FAR_CLIPPING_PLANE = -1.f;
    mat4x4_ortho(p, LEFT_CLIPPING_PLANE, RIGHT_CLIPPING_PLANE, BOTTOM_CLIPPING_PLANE, TOP_CLIPPING_PLANE, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE);

    // constexpr float FOV_IN_RANGLES = 1.57f; // 90 degrees in radians
    // constexpr float NEAR_PLANE = 1.f;
    // constexpr float FAR_PLANE = 10.f;
    // mat4x4_perspective(p, FOV_IN_RANGLES, m_fRatio, NEAR_PLANE, FAR_PLANE);

    mat4x4_mul(oOutViewMatrix, p, m);
}
