#version 330 core
out vec4 oFragColor;

in vec3 ioNormal;
in vec3 ioFragWorldPos;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;


void main()
{
    // ambient
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * uLightColor;
    
    // diffuse
    vec3 norm = normalize(ioNormal);
    vec3 light_ray = normalize(uLightPos - ioFragWorldPos);
    float diff = max(dot(norm, light_ray), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 result = (ambient + diffuse)* uObjectColor;
    oFragColor = vec4(result, 1.0);
}
