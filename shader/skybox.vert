#version 330

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 vertexPosition;

out vec3 texCoord;

void main()
{
	texCoord = vertexPosition;

	gl_Position = projMatrix * viewMatrix * vec4(vertexPosition, 1.0);
}