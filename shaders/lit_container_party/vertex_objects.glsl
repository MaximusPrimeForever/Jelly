#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iTexCoords;

out vec3 ioNormal;
out vec3 ioFragViewPos;
out vec2 ioTexCoords;

out vec3 ioLightViewPos;
out vec3 ioLightViewDir;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

uniform vec3 uLightPos;
uniform vec3 uLightDir;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(iPos, 1.0);

    // Compute view space positions
    ioFragViewPos = (uView * uModel * vec4(iPos, 1.0)).xyz;
    ioLightViewPos = (uView * vec4(uLightPos, 1.0)).xyz;

    // Compute light direction in view space, without translation because it's a direction
    ioLightViewDir = (uView * vec4(uLightDir, 0.0)).xyz;

    // uNormalMatrix is derived from view*model in CPU code
    ioNormal =  uNormalMatrix * iNormal;
    ioTexCoords = iTexCoords;
}
