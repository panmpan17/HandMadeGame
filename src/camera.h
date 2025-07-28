#include <linmath.h>


class Camera {
public:
    Camera();
    ~Camera();

    inline void setRatio(float fRatio) {
        m_fRatio = fRatio;
    }
    inline void setPosition(float fX, float fY, float fZ) {
        m_position[0] = fX;
        m_position[1] = fY;
        m_position[2] = fZ;
    }
    inline void setRotation(float fX, float fY, float fZ) {
        m_rotation[0] = fX;
        m_rotation[1] = fY;
        m_rotation[2] = fZ;
    }
    inline void rotate(float fX, float fY, float fZ) {
        m_rotation[0] += fX;
        m_rotation[1] += fY;
        m_rotation[2] += fZ;
    }

    void getViewMatrix(mat4x4& oOutViewMatrix);

private:
    float m_fRatio = 1.0f;
    vec3 m_position = {0.f, 0.f, 0.f};
    vec3 m_rotation = {0.f, 0.f, 0.f};
    mat4x4 m_projectionMatrix = {0};
};
