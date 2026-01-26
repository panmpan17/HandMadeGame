#include <metal_stdlib>

#ifndef MATH_METAL
#define MATH_METAL

metal::float4x4 mat3ToMat4(metal::float3x3 mat3) {
    metal::float4x4 mat4 = metal::float4x4(1.0);
    mat4.columns[0].xyz = mat3.columns[0];
    mat4.columns[1].xyz = mat3.columns[1];
    mat4.columns[2].xyz = mat3.columns[2];
    return mat4;
}

metal::float3x3 mat4ToMat3(metal::float4x4 mat4) {
    return metal::float3x3(mat4.columns[0].xyz,
                          mat4.columns[1].xyz,
                          mat4.columns[2].xyz);
}

#endif // MATH_METAL