#version 330 core
out vec4 FragColor;
in vec3 Normal;

// posto je izvor svetla beo, a znamo koje boje hocu da budu ostali podijumi, nemoram da saljem nista za anbijentalnu svetlost

// za difuzno svetlo nam treba vektor normale svakog objekta koji biva osvetljen tj koji crta ovaj shader
// i treba nam pozicija izvora svetlosti (ima ih 4)

// znaci ja sad treba da raspadnem EBO i da uz svaki vertex saljem normalu na njega, A POSLE ce nam pakazati kako to da smanjimo fuck off

void main()
{
    FragColor = vec4(0.7f, 0.5f, 0.5f, 1.0f);
}