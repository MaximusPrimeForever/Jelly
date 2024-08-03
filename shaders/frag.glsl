#version 330 core
in vec4 vertex_color;

out vec4 frag_color;

//uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main()
{
    frag_color = vertex_color;
}
