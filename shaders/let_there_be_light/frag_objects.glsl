#version 330 core
out vec4 oFragColor;

in vec2 ioTexCoords;
in vec3 ioNormal;
in vec3 ioFragViewPos;
in vec3 ioLightPos;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Light uLight;
uniform Material uMaterial;


void main()
{    
    vec3 norm = normalize(ioNormal);
    vec3 viewLightPos = ioLightPos;

    // incident ray points from light to fragment
    vec3 incidentRay = normalize(ioFragViewPos - ioLightPos);
    vec3 cameraToFrag = normalize(-ioFragViewPos);

    // diffuse
    float diff = max(dot(norm, -incidentRay), 0.0);

    // specular
    // reflect == incidentRay - 2 * norm * dot(norm, incidentRay);
    vec3 reflected = reflect(incidentRay, norm);
    float spec = pow(max(dot(cameraToFrag, reflected), 0.0), uMaterial.shininess);

    vec3 fragDiffColor = vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 fragSpecColor = vec3(texture(uMaterial.specular, ioTexCoords));

    // crop emission texture
    vec2 emisCoord = (ioTexCoords * 1.2) - 0.1;

    vec3 ambient =  uLight.ambient  * fragDiffColor;
    vec3 diffuse =  uLight.diffuse  * (diff * fragDiffColor);
    vec3 specular = uLight.specular * (spec * fragSpecColor);

    vec3 result = ambient + diffuse + specular;
    oFragColor = vec4(result, 1.0);
}
