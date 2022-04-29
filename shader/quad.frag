#version 330

uniform sampler2D renderTexture;
in vec2 texCoord;
out vec4 fragColor;

void main()
{
    const float gamma = 0.6;

	vec3 texColor = texture(renderTexture, texCoord).rgb;

    // Reinhard tone mapping
    vec3 mapped = texColor / (texColor + vec3(1.0));
    // Gamma correction
    texColor = pow(texColor, vec3(1.0 / gamma));

	fragColor = vec4(texColor, 1.0);
}