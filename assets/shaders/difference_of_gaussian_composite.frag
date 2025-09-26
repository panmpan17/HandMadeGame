#version 330

uniform sampler2D u_originalTexture;
uniform sampler2D u_blur1Texture;
uniform sampler2D u_blur2Texture;

in vec2 uv;

out vec4 fragment;

void main()
{
    vec4 textColor1 = texture(u_blur1Texture, uv);
    vec4 textColor2 = texture(u_blur2Texture, uv);

    vec3 diff = textColor1.rgb - textColor2.rgb;
    // fragment = vec4(diff, 1.0);

    // float lumSmall = dot(textColor1.rgb, vec3(0.299, 0.587, 0.114));
    // float lumLarge = dot(textColor2.rgb, vec3(0.299, 0.587, 0.114));
    // float dog = lumSmall - lumLarge;
    float dog = (diff.r + diff.g + diff.b) / 3.0;

    vec4 originalColor = texture(u_originalTexture, uv);
    float lumOrg = (originalColor.r + originalColor.g + originalColor.b) / 3.0;

    // if (dog < 0.01) {
    //     fragment = vec4(vec3(lumOrg), 1.0);
    // //     fragment = originalColor;
    //     // fragment = vec4(0);

    // //     dog = 0.0;
    // }
    // else {
        // vec4 originalColor = texture(u_originalTexture, uv);

        // dog *= 2.0;
        // if (dog > 1.0)
        //     dog = 1.0;

        fragment = vec4(vec3(lumOrg) + diff * 3, 1.0);
    // }
    //     dog = 1.0;

    // fragment = vec4(vec3(dog), 1.0);

    // fragment = vec4(diff * 5.0 + textColor1.rgb, 1.0);
}