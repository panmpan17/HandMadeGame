#include <metal_stdlib>

struct CameraMatrices {
    metal::float4x4 view;
    metal::float4x4 projection;
    metal::float4 position;
};