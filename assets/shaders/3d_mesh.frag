#version 330

uniform sampler2D u_MainTex;

in vec2 fragUV;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragment;

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0) - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

    vec3 ambient = 0.2 * vec3(.2, .2, .2);

    vec4 texColor = texture(u_MainTex, fragUV);
    diffuse *= texColor.rgb;

    fragment = vec4(texColor.rgb, 1);
}