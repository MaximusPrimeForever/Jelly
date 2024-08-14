#version 330 core
layout (location = 0) in vec3 iPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 newPosition = vec4(iPos, 1.0);
    gl_Position = uProjection * uView * uModel * newPosition;
}
