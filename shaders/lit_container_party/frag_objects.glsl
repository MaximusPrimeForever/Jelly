#version 330 core
out vec4 oFragColor;

in vec2 ioTexCoords;
in vec3 ioNormal;
in vec3 ioFragViewPos;
in vec3 ioLightViewPos;

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform DirectionalLight uDirLight;
uniform PointLight uPointLight;
uniform Material uMaterial;


void main()
{    
    vec3 norm = normalize(ioNormal);

    // incident ray points from light to fragment
    vec3 incidentRayDirectional = normalize(-uDirLight.direction);

    vec3 incidentRayPoint = normalize(ioFragViewPos - ioLightViewPos);

    vec3 incidentRay = incidentRayPoint;
    vec3 cameraToFrag = normalize(-ioFragViewPos);

    // diffuse
    float diff = max(dot(norm, -incidentRay), 0.0);

    // specular
    vec3 reflected = reflect(incidentRay, norm);
    float spec = pow(max(dot(cameraToFrag, reflected), 0.0), uMaterial.shininess);

    vec3 fragDiffColor = vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 fragSpecColor = vec3(texture(uMaterial.specular, ioTexCoords));

    vec3 ambient =  uPointLight.ambient  * fragDiffColor;
    vec3 diffuse =  uPointLight.diffuse  * (diff * fragDiffColor);
    vec3 specular = uPointLight.specular * (spec * fragSpecColor);

    // attenuation
    float dist = length(ioFragViewPos - ioLightViewPos);
    float attenuation = 1.0 / (
          uPointLight.constant
        + uPointLight.linear * dist
        + uPointLight.quadratic * pow(dist, 2)
    );

    vec3 result = (ambient + diffuse + specular) * attenuation;
    oFragColor = vec4(result, 1.0);
}
