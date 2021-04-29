#version 330 core

struct Material{
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec3 viewPos;
in vec2 texCoords;
out vec4 FragColor;

// uniform vec3 viewPos;

void main()
{
    // there are no shadows?
    vec3 lightColor = vec3(1.0f);

    // abijentalno
    vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb;

    // difuzno
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoords).rgb;

    // spekularno - nema ga, kapiram zbog camera position
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec3 specular = light.specular * (spec * texture(material.specular, texCoords).rgb);

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);
}