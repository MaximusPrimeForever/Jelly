#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex;

out vec2 tex_coord;
out vec4 vertex_color; // specify a color output to the fragment shader

uniform float horizontal_offset; // we set this variable in the OpenGL code.
uniform mat4 trans;

void main()
{
    vec3 new_position = vec3(pos.x + horizontal_offset, pos.y, pos.z);
    gl_Position = trans * vec4(new_position, 1.0);
    vertex_color = vec4(color, 1.0);
    tex_coord = tex;
}
