#version 330 core
out vec4 frag_color;

in vec4 vertex_color;
in vec2 tex_coord;

uniform sampler2D tex0_data;
uniform sampler2D tex1_data;

void main()
{
    frag_color = mix(texture(tex0_data, tex_coord), texture(tex1_data, tex_coord), 0.2);
}
