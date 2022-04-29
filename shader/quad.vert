#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord_in;

out vec2 texCoord;

void main()
{
	texCoord = texCoord_in;
	gl_Position = vec4(vertexPosition, 1.0);
}