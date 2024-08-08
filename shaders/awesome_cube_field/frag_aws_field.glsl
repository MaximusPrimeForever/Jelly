// Fragment shader for Awesome Cube

#version 330 core
out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D tex0_data;
uniform sampler2D tex1_data;

void main()
{
    frag_color = mix(texture(tex0_data, tex_coords), texture(tex1_data, tex_coords), 0.2);
}
