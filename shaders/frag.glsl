#version 330 core
in vec4 vertexColor;

out vec4 fragColor;

//uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main()
{
    fragColor = vertexColor;
}
