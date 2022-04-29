#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPosition;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord_in;

out vec4 clipspace_coord;
out vec2 uv_coord;
out vec3 toCameraVector;

void main()
{
    uv_coord = texCoord_in * 10;
    vec4 world_position = modelMatrix * vec4(vertexPosition, 1.0);
	clipspace_coord = projMatrix * viewMatrix * world_position;
	gl_Position = clipspace_coord;
	toCameraVector = cameraPosition - vec3(world_position);
}