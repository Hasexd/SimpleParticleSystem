#version 450 core

layout (location = 0) in vec3 inPosition;

uniform mat4 viewProj;
uniform mat4 uTransform;

void main()
{
	gl_Position = viewProj * uTransform * vec4(inPosition, 1.0);
}