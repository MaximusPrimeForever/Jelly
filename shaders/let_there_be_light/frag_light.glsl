#version 330 core
out vec4 oFragColor;

uniform vec3 uLightColor;

void main()
{
	oFragColor = vec4(uLightColor, 1.0);
}
