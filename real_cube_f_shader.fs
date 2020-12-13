#version 330 core
out vec4 FragColor;

in unit {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} unit_fs;

uniform sampler2D texture_real_cube;		// diffuseMap;	
uniform sampler2D texture_normal_cube;		//normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{           
     // получаем нормаль из текстуры нормалей
    vec3 normal = texture(texture_normal_cube, unit_fs.TexCoords).rgb;
    // преобразуем вектор
    normal = normalize(normal * 2.0 - 1.0);  // в касательной области
   
    // получаем диффузный цвет
    vec3 color = texture(texture_real_cube, unit_fs.TexCoords).rgb;
    
    // фоновой
    vec3 ambient = 0.1 * color;

    // диффузный
    vec3 lightDir = normalize(unit_fs.TangentLightPos - unit_fs.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // зеркальный
    vec3 viewDir = normalize(unit_fs.TangentViewPos - unit_fs.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}