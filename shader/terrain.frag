#version 330

uniform sampler2D maskTex;
uniform sampler2D xTex;
uniform sampler2D yTex;
uniform sampler2D zTex;
uniform sampler2D wTex;
uniform sampler2D xTex_normal;
uniform sampler2D yTex_normal;
uniform sampler2D zTex_normal;
uniform sampler2D wTex_normal;
uniform float xSpec;
uniform float ySpec;
uniform float zSpec;
uniform float wSpec;


in vec2 texCoords;
in vec2 terrainCoords;
in vec3 lightDir_tangentSpace;
in vec3 eyeDir_tangentSpace;

out vec4 fragColor;

void main()
{
    float m = 0.1;
    float F0 = 1.0;
    vec4 mask = texture(maskTex, terrainCoords);

    vec4 diffuseColor = (mask.x * texture(xTex, texCoords) + mask.y * texture(yTex, texCoords) +
                         mask.z * texture(zTex, texCoords) + (1 - mask.w) * texture(wTex, texCoords));
    float specularStrength = (mask.x * xSpec + mask.y * ySpec + mask.z * zSpec + (1 - mask.w) * wSpec);
    specularStrength /= (mask.x + mask.y + mask.z + (1 - mask.w));
    // now normalize
    diffuseColor /= (mask.x + mask.y + mask.z + (1 - mask.w));

    vec4 normal_tangentspace = (mask.x * texture(xTex_normal, texCoords) + mask.y * texture(yTex_normal, texCoords) +
                                mask.z * texture(zTex_normal, texCoords) + (1 - mask.w) * texture(wTex_normal, texCoords));
    normal_tangentspace /= (mask.x + mask.y + mask.z + (1 - mask.w));
    normal_tangentspace = normal_tangentspace * 2 - 1;
    vec3 normal_tangentspace_xyz = normalize(normal_tangentspace.xyz);
    float NdotL = max(0.0, dot(normal_tangentspace_xyz, normalize(lightDir_tangentSpace)));
    vec3 color = diffuseColor.rgb * (0.4 + 1.2 * NdotL);
    if (NdotL > 0) {
        vec3 halfVector = normalize(normalize(eyeDir_tangentSpace) + normalize(lightDir_tangentSpace));
        float NdotH = max(dot(normal_tangentspace_xyz, halfVector), 0.0);
        float NdotV = max(dot(normal_tangentspace_xyz, normalize(eyeDir_tangentSpace)), 0.0); // note: this could also be NdotL, which is the same value
        float VdotH = max(dot(normalize(eyeDir_tangentSpace), halfVector), 0.0);
        float mSquared = m * m;


        // geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));

        // roughness (or: microfacet distribution function)
        // beckmann distribution function
        float r1 = 1.0 / (3.14 * mSquared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
        float roughness = r1 * exp(r2);

        // fresnel
        // Schlick approximation
        float fresnel = pow(1.0 - VdotH, 5.0);
        fresnel *= (1.0 - F0);
        fresnel += F0;
        color += 2 * specularStrength * max(NdotL * (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14), 0.0);
    }

    //fragColor = vec4(normalize(lightDir_tangentSpace), 1.0);
    fragColor = vec4(color, 1.0);
}
