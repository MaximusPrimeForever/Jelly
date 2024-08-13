#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offset;

void main()
{
    vec4 new_position = vec4(pos + offset, 1.0);
    gl_Position = projection * view * model * new_position;
}
