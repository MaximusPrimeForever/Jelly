#version 330 core
layout (location = 0) in vec2 iPos; // contains x,z coords
layout (location = 1) in vec2 iTex_coord; // contains x,z coords

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(iPos.x, 0.0, iPos.y, 1.0);
    tex_coord = iTex_coord;
}
