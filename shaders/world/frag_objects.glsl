#version 330 core

out vec4 oFragColor;

in vec3 ioNormal;
in vec2 ioTexCoords;
in vec3 ioFragViewPos;

#define LIGHT_COUNT (4)

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight uDirLight;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight uPointLights[LIGHT_COUNT];

struct SpotLight {
    vec3 position;
    vec3 direction;

    float innerCutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight uFlashlight;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 fragViewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragViewDir); 
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 fragViewDir);


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material uMaterial;

uniform mat4 uModel;
uniform mat4 uView;


void main()
{    
    vec3 viewDir = normalize(-ioFragViewPos);
    vec3 material_diffuse = vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 material_specular = vec3(texture(uMaterial.specular, ioTexCoords));

    vec3 result = vec3(0.0);
    result += CalcDirLight(uDirLight, ioNormal, ioFragViewPos, viewDir);
    
    for(int i = 0; i < LIGHT_COUNT; i++) {
        result += CalcPointLight(
            uPointLights[i],
            ioNormal, 
            ioFragViewPos, 
            viewDir
        );
    }

    result += CalcSpotLight(
        uFlashlight,
        ioNormal,
        ioFragViewPos,
        viewDir
    );
    oFragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(
    DirectionalLight light, 
    vec3 normal, 
    vec3 fragPos, 
    vec3 fragViewDir
) {
    // convert to view space
    vec3 incidentRay = normalize((uView * vec4(light.direction, 0.0)).xyz);

    // diffuse
    float diffuseFact = max(dot(-incidentRay, normal), 0.0);

    // specular
    vec3 reflectedRay = reflect(incidentRay, normal);
    float specularFact = pow(
        max(dot(fragViewDir, reflectedRay), 0.0),
        uMaterial.shininess
    );

    vec3 ambient = light.ambient * vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 diffuse = light.diffuse * diffuseFact * vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 specular = light.specular * specularFact * vec3(texture(uMaterial.specular, ioTexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(
    PointLight light,
    vec3 normal, 
    vec3 fragPos, 
    vec3 fragViewDir
) {
    // convert to view space
    vec3 lightViewPos = (uView * vec4(light.position, 1.0)).xyz;
    vec3 incidentRay = normalize(ioFragViewPos - lightViewPos);

    vec3 fragDiffColor = vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 fragSpecColor = vec3(texture(uMaterial.specular, ioTexCoords));

    // diffuse
    float diff = max(dot(normal, -incidentRay), 0.0);

    // specular
    vec3 reflected = reflect(incidentRay, normal);
    float spec = pow(
        max(dot(fragViewDir, reflected), 0.0),
        uMaterial.shininess
    );

    vec3 ambient =  light.ambient  * fragDiffColor;
    vec3 diffuse =  light.diffuse  * (diff * fragDiffColor);
    vec3 specular = light.specular * (spec * fragSpecColor);

    // attenuation
    float dist = length(ioFragViewPos - lightViewPos);
    float attenuation = 1.0 / (
          light.constant
        + light.linear * dist
        + light.quadratic * pow(dist, 2)
    );

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(
    SpotLight light, 
    vec3 normal, 
    vec3 fragPos, 
    vec3 fragViewDir
) {
    // convert to view space
    vec3 lightViewPos = (uView * vec4(light.position, 1.0)).xyz;
    vec3 lightViewDir = normalize((uView * vec4(light.direction, 0.0)).xyz);
    vec3 incidentRay = normalize(ioFragViewPos - lightViewPos);

    // diffuse
    float diff = max(dot(normal, -incidentRay), 0.0);

    // specular
    vec3 reflected = reflect(incidentRay, normal);
    float spec = pow(max(dot(fragViewDir, reflected), 0.0), uMaterial.shininess);

    vec3 fragDiffColor = vec3(texture(uMaterial.diffuse, ioTexCoords));
    vec3 fragSpecColor = vec3(texture(uMaterial.specular, ioTexCoords));

    // spotlight check
    // theta is the angle between light<>frag, and spotlight direction
    float theta = dot(incidentRay, lightViewDir);
    float cutoffDiff = light.innerCutoff - light.outerCutoff;
    float intensity = clamp(
        (theta - light.outerCutoff) / cutoffDiff,
        0.0,
        1.0
    );

    vec3 ambient =  light.ambient  * fragDiffColor;
    vec3 diffuse =  light.diffuse  * (diff * fragDiffColor);
    vec3 specular = light.specular * (spec * fragSpecColor);

    // attenuation
    float dist = length(ioFragViewPos - lightViewPos);
    float attenuation = 1.0 / (
          light.constant
        + light.linear * dist
        + light.quadratic * pow(dist, 2)
    );

    return (ambient + diffuse + specular) * attenuation * intensity;
}
