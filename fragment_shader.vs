#version 330 core
struct Material {
    sampler2D diffuse;	// рассеянный свет
    sampler2D specular;	// бликовой свет
    float shininess;	// сила блеска
}; 
  
struct Light {
    //vec3 position;	// не нужен для направленного источника
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light; 
uniform Material material;
uniform vec3 viewPos;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

// uniform vec3 objectColor;	// цвет объекта
// uniform vec3 lightColor = vec3(1.0)	// цвет источника света (чтобы не засвечивался)
// uniform vec3 lightPos; 
 



//uniform sampler2D ourTexture1;
//uniform sampler2D ourTexture2;

void main()
{    
    // фоновое освещение
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);	// вектор направления от источника (направленный)
    float diff = max(dot(norm, lightDir), 0.0);

    // диффузное освещение
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));  

    // бликовое освещение
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));  

    // vec3 result = vec4(ambient + diffuse + specular, 1.0);
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}