#version 330 core
out vec4 oFragColor;

in vec3 ioNormal;
in vec3 ioFragViewPos;
in vec3 ioLightPos;

in mat4 ioViewModelMat;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    vec3 ambient = ambientStrength * uLightColor;
    
    // diffuse
    vec3 norm = normalize(ioNormal);
    vec3 viewLightPos = ioLightPos;

    // incident ray points from light to fragment
    vec3 incident_ray = normalize(ioFragViewPos - viewLightPos);
    vec3 cameraToFrag = normalize(-ioFragViewPos);
    
    vec3 diffuse = max(dot(norm, -incident_ray), 0.0) * uLightColor;

    // reflect ==  incident_ray - 2 * norm * dot(norm, incident_ray);
    vec3 reflected = reflect(incident_ray, norm);

    float spec = pow(max(dot(cameraToFrag, reflected), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor;

    vec3 result = (ambient + diffuse + specular) * uObjectColor;

    oFragColor = vec4(result, 1.0);
}
