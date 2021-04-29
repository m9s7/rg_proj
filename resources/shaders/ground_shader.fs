#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 viewPos;
out vec4 FragColor;

uniform vec3 lightPos;
// uniform vec3 viewPos;

// mogu da dodam strukture za materijal i svetlo <jacine svake komponente + lightPos>

void main()
{
    // there are no shadows?
    vec3 lightColor = vec3(1.0f);
    vec3 objColor = vec3(0.7f, 0.5f, 0.5f);

    // abijentalno
    float ambientStrength = 0.2f;
    vec3 ambient = lightColor * ambientStrength;

    // difuzno
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // spekularno - nema ga, kapiram zbog camera position
    float specularStrength = 1.0f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec3 specular = spec * specularStrength * lightColor;

    vec3 result = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(result, 1.0f);
}