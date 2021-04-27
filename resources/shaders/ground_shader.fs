#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec3 viewPos;

// posto je izvor svetla beo, a znamo koje boje hocu da budu ostali podijumi, nemoram da saljem nista za anbijentalnu svetlost

// za difuzno svetlo nam treba vektor normale svakog objekta koji biva osvetljen tj koji crta ovaj shader
// i treba nam pozicija izvora svetlosti (ima ih 4)

// znaci ja sad treba da raspadnem EBO i da uz svaki vertex saljem normalu na njega, A POSLE ce nam pakazati kako to da smanjimo fuck off

uniform vec3 lightPos;
// uniform vec3 viewPos;

void main()
{
    // there are no shadows?
    vec3 lightColor = vec3(1.0f);

    float ambientStrength = 0.3f;
    vec3 ambient = lightColor * ambientStrength;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 1.0f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec3 specular = spec * specularStrength * lightColor;

    vec3 result = (ambient + diffuse + specular) * vec3(0.7f, 0.5f, 0.5f);
    FragColor = vec4(result, 1.0f);

//     FragColor = vec4(0.7f, 0.5f, 0.5f, 1.0f);
}