#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color; // the color variable has attribute position 1

out vec4 vertex_color; // specify a color output to the fragment shader

uniform float horizontal_offset; // we set this variable in the OpenGL code.

void main()
{
    gl_Position = vec4(pos.x + horizontal_offset, pos.y * -1, pos.z, 1.0);
    vertex_color = vec4(color, 1.0);
}
