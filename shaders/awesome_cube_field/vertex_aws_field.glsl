// Vertex shader for Awesome Cube

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex_coords;

out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 new_position = aPos;
    gl_Position = projection * view * model * vec4(new_position, 1.0);
    tex_coords = aTex_coords;
}
