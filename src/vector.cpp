#include "vector.h"
#include "window.h"
#include "camera.h"
#include <linmath.h>
#include <string>
#include <stdexcept>


Vector2 WorldVectorSytem::screenToWorld(float screenX, float screenY)
{
    /*
    int windowWidth, windowHeight;
    m_pWindow->getWindowSize(windowWidth, windowHeight);

    if (windowWidth <= 0 || windowHeight <= 0)
    {
        // Handle invalid window size
        return Vector2(0, 0);
    }

    // 1. Convert screen coordinates (top-left origin, Y-down) to NDC (center origin, Y-up)
    float ndcX = (2.0f * screenX) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY) / windowHeight;

    // Create a 4D vector in NDC space
    // For 2D, we assume the Z position is at the origin of NDC [-1, 1], so Z=0
    vec4 ndcVector = {ndcX, ndcY, 0.0f, 1.0f};
    */

    // 2. Get the combined inverse matrix (Inverse(Projection * View))
//     mat4x4 pvMatrix;
//     mat4x4_mul(pvMatrix, m_pCamera->getProjectionMatrix(), m_pCamera->getViewProjectionMatrix());

//     mat4x4 inversePVMatrix;
//     mat4x4_invert(inversePVMatrix, pvMatrix);
    
//     // 3. Unproject from NDC to World Space
//     vec4 worldVector;
//     mat4x4_mul_vec4(worldVector, inversePVMatrix, ndcVector);

//     // Perform the perspective divide (for orthographic, w should be 1.0)
//     if (worldVector[3] != 0.0f) {
//         worldVector[0] /= worldVector[3];
//         worldVector[1] /= worldVector[3];
//         // Don't need to divide z for 2D, but good practice
//     }

//     return {worldVector[0], worldVector[1]};
    throw std::runtime_error("Unimplemented: WorldVectorSytem::screenToWorld");
}

