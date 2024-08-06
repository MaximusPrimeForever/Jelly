#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_box;
layout (location = 3) in vec2 tex_face;

out vec4 vertex_color; // specify a color output to the fragment shader
out vec2 tex_coord_box;
out vec2 tex_coord_face;

void main()
{
    vec3 new_position = vec3(pos.x, pos.y, pos.z);
    gl_Position = vec4(new_position, 1.0);
    vertex_color = vec4(color, 1.0);
    tex_coord_box = tex_box;
    tex_coord_face = tex_face;
}
