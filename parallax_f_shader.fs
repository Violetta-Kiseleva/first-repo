#version 330 core
out vec4 FragColor;

in unit {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} unit_fs;

uniform sampler2D texture_parallax_diffuse;
uniform sampler2D texture_parallax_normal;
uniform sampler2D texture_parallax_height;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // количество слоев глубины
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    // вычисляем размер каждого слоя
    float layerDepth = 1.0 / numLayers;

    // глубина текущего слоя
    float currentLayerDepth = 0.0;

    // величина сдвига текстурных координат для каждого слоя (из вектора P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;

    // получаем начальные значения
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(texture_parallax_height, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        // сдвигаем текстурные координаты вдоль направления вектора P
        currentTexCoords -= deltaTexCoords;

        // получаем значение глубины из карты глубины для текущих текстурных координат
        currentDepthMapValue = texture(texture_parallax_height, currentTexCoords).r; 

        // получаем значение глубины следующего слоя
        currentLayerDepth += layerDepth;  
    }


//////////////// 		RELIEF				////////////////

    // уполовиниваем смещение текстурных координат и размер слоя глубины
	deltaTexCoords *= 0.5;
	layerDepth *= 0.5;
    // сместимся в обратном направлении от точки, найденной в Steep PM
	currentTexCoords += deltaTexCoords;
	currentLayerDepth -= layerDepth;

    // установим максимум итераций поиска…
	const int _reliefSteps = 5;
	int currentStep = _reliefSteps;
	while (currentStep > 0) {
		currentDepthMapValue = texture(texture_parallax_height, currentTexCoords).r;
		deltaTexCoords *= 0.5;
		layerDepth *= 0.5;
    // если выборка глубины больше текущей глубины слоя, 
    // то уходим в левую половину интервала
		if (currentDepthMapValue > currentLayerDepth) {
			currentTexCoords -= deltaTexCoords;
			currentLayerDepth += layerDepth;
		}
    // иначе уходим в правую половину интервала
		else {
			currentTexCoords += deltaTexCoords;
			currentLayerDepth -= layerDepth;
		}
		currentStep--;
	}

	float lastDepthValue = currentDepthMapValue;

    return currentTexCoords;
}

void main()
{           
    // смещение текстурных координат при использовании Параллакс Отображения
    vec3 viewDir = normalize(unit_fs.TangentViewPos - unit_fs.TangentFragPos);
    vec2 texCoords = unit_fs.TexCoords;

    texCoords = ParallaxMapping(unit_fs.TexCoords,  viewDir);       
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // получение нормали из карты нормалей
    vec3 normal = texture(texture_parallax_normal, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   

    // получение диффузного цвета
    vec3 color = texture(texture_parallax_diffuse, texCoords).rgb;

    // фоновая составляющая
    vec3 ambient = 0.1 * color;

    // диффузная составляющая
    vec3 lightDir = normalize(unit_fs.TangentLightPos - unit_fs.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // отраженная составляющая    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}