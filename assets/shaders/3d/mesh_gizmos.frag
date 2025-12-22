#version 330

uniform vec4 u_GizmosColor;

out vec4 fragment;

void main()
{
    fragment = u_GizmosColor;
}
