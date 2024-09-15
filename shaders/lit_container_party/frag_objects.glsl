#version 330 core
out vec4 oFragColor;

in vec3 ioNormal;
in vec2 ioTexCoords;
in vec3 ioFragViewPos;

in vec3 ioLightViewPos;
in vec3 ioLightViewDir;

struct SpotLight {
    float innerCutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight uSpotLight;

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight uPointLight;

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight uDirLight;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material uMaterial;


void main()
{    
    vec3 norm = normalize(ioNormal);

    // incident ray points from light to fragment
//    vec3 incidentRayDirectional = normalize(-uDirLight.direction);

    vec3 incidentRay = normalize(ioFragViewPos - ioLightViewPos);
    vec3 cameraToFrag = normalize(-ioFragViewPos);

    // diffuse
    float diff = max(dot(norm, -incidentRay), 0.0);

    // specular
    vec3 reflected = reflect(incidentRay, norm);
    float spec = pow(max(dot(cameraToFrag, reflected), 0.0), uMaterial.shininess);

    vec3 fragDiffColor = vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 fragSpecColor = vec3(texture(uMaterial.specular, ioTexCoords));

    // spotlight check
    // theta is the angle between light<>frag, and spotlight direction
    float theta = dot(
        normalize(ioLightViewDir),
        incidentRay
    );
    float cutoffDiff = uSpotLight.innerCutoff - uSpotLight.outerCutoff;
    float intensity = clamp(
        (theta - uSpotLight.outerCutoff) / cutoffDiff,
        0.0,
        1.0
    );

    vec3 ambient =  uSpotLight.ambient  * fragDiffColor;
    vec3 diffuse =  uSpotLight.diffuse  * (diff * fragDiffColor);
    vec3 specular = uSpotLight.specular * (spec * fragSpecColor);

    // attenuation
    float dist = length(ioFragViewPos - ioLightViewPos);
    float attenuation = 1.0 / (
          uSpotLight.constant
        + uSpotLight.linear * dist
        + uSpotLight.quadratic * pow(dist, 2)
    );

    vec3 result = ambient + (diffuse + specular) * attenuation * intensity;
    oFragColor = vec4(result, 1.0);
}
