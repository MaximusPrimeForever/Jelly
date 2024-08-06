#version 330 core
out vec4 frag_color;

in vec4 vertex_color;
in vec2 tex_coord_box;
in vec2 tex_coord_face;

uniform sampler2D tex0_data;
uniform sampler2D tex1_data;
uniform float mix_value;

void main()
{
    vec2 flipped = vec2(-tex_coord_face.x, tex_coord_face.y);
    frag_color = mix(texture(tex0_data, tex_coord_box), texture(tex1_data, flipped), mix_value);
}
