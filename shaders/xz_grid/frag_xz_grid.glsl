#version 330 core
out vec4 frag_color;

in vec2 tex_coord;

uniform sampler2D tex;

void main()
{
	vec4 tex_color = texture(tex, tex_coord);
	frag_color = tex_color;
}
