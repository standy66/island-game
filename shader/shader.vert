#version 330

uniform mat4 modelMatrix;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

out vec4 color;

void main()
{
    color = vertexColor;
    gl_Position = modelMatrix * vec4(vertexPosition, 1.0);
}
