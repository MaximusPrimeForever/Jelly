#version 330 core
out vec4 frag_color;

in vec4 vertex_color;
in vec2 tex_coord;

uniform sampler2D tex_data;
//uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main()
{
    frag_color = texture(tex_data, tex_coord);
//    frag_color = vertex_color;
}
