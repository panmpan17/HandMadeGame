#pragma once

#include <linmath.h>

typedef struct Vertex
{
    vec2 pos;
} Vertex;

typedef struct VertexWColor
{
    vec2 pos;
    vec3 col;
} VertexWColor;

typedef struct VertexWUV
{
    vec2 pos;
    vec2 uv;
} VertexWUV;

typedef struct VertexWUVNormal
{
    vec3 pos;
    vec2 uv;
    vec3 normal;
} VertexWUVNormal;
