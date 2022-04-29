#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 clipping_plane;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec3 vertexBitangent;
layout(location = 4) in vec2 texCoords_in;
layout(location = 5) in vec2 terrainCoords_in;

out vec2 texCoords;
out vec2 terrainCoords;
out vec3 lightDir_tangentSpace;
out vec3 eyeDir_tangentSpace;

void main()
{
    vec4 world_position = modelMatrix * vec4(vertexPosition, 1.0);
    gl_ClipDistance[0] = dot(clipping_plane, world_position);
    mat4 MV = viewMatrix * modelMatrix;
    mat4 MVP = projMatrix * MV;
    vec3 vertexTangent_cameraspace = mat3(MV) * vertexTangent;
    vec3 vertexBitangent_cameraspace = mat3(MV) * vertexBitangent;
    vec3 vertexNormal_cameraspace = mat3(MV) * vertexNormal;
    mat3 TBN = transpose(mat3(
             vertexTangent_cameraspace,
             vertexBitangent_cameraspace,
             vertexNormal_cameraspace));

    lightDir_tangentSpace = TBN * mat3(MV) * vec3(1.0, 1.0, 0.0);
    eyeDir_tangentSpace = TBN * vec3(0.0, 0.0, -1.0);

    texCoords = texCoords_in;
    terrainCoords = terrainCoords_in;
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}
