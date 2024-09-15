#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iTexCoords;

out vec3 ioNormal;
out vec2 ioTexCoords;
out vec3 ioFragViewPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;


void main()
{
    gl_Position = uProjection * uView * uModel * vec4(iPos, 1.0);

    // Compute view space positions
    ioFragViewPos = (uView * uModel * vec4(iPos, 1.0)).xyz;

    // uNormalMatrix is derived from view*model in CPU code
    ioNormal =  normalize(uNormalMatrix * iNormal);
    ioTexCoords = iTexCoords;
}
