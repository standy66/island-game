#version 330

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform sampler2D dudv_texture;
uniform sampler2D normal_texture;
uniform float offset;

in vec4 clipspace_coord;
in vec3 toCameraVector;
in vec2 uv_coord;
out vec4 fragColor;

void main()
{
    vec2 ndc = (clipspace_coord.xy / clipspace_coord.w) / 2.0 + 0.5;
    vec2 distortedTexCoords = texture(dudv_texture, vec2(uv_coord.x + offset, uv_coord.y)).rg*0.1;
	distortedTexCoords = uv_coord + vec2(distortedTexCoords.x, distortedTexCoords.y+offset);
	vec2 totalDistortion = (texture(dudv_texture, distortedTexCoords).rg * 2.0 - 1.0) * 0.005;
    vec3 normal = texture(normal_texture, distortedTexCoords).rgb * 2 - 1;
    float NdotL = dot(normalize(vec3(1.0, 1.0, 0.0)), normalize(normal));
    ndc = clamp(ndc + totalDistortion, 0, 1);

	vec3 refraction_color = texture(refraction_texture, ndc).rgb;
    ndc.y = 1 - ndc.y;
	vec3 reflection_color = texture(reflection_texture, ndc).rgb;
	float reflective_index = 1-dot(normalize(-toCameraVector), vec3(0.0, 1.0, 0.0));
	reflective_index = pow(reflective_index, 2.2);
    //	if (reflective_index < 0 || reflective_index > 1) {
    //	    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //	} else {
    //	//reflective_index *= reflective_index;
    //	//fragColor = vec4(reflective_index * reflection_color + refraction_color, 1.0);
    //	    fragColor = vec4(reflective_index, reflective_index, reflective_index, 1.0);
    //	}
    vec3 color = mix(refraction_color, reflection_color, reflective_index);
    if (NdotL > 0) {
        color += 0.3 * NdotL;
    }
    fragColor = vec4(color, 1.0);
}