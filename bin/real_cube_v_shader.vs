#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out unit {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} unit_vs;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    unit_vs.FragPos = vec3(model * vec4(aPos, 1.0));   
    unit_vs.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));

    // преобразуем все вектора базиса в нашу систему координат
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);	// вектора TNB взаимно перпендикулярны
    
    // создаем матрицу TBN, передав конструктуру все три вектора
    mat3 TBN = transpose(mat3(T, B, N));    
    unit_vs.TangentLightPos = TBN * lightPos;
    unit_vs.TangentViewPos  = TBN * viewPos;
    unit_vs.TangentFragPos  = TBN * unit_vs.FragPos;
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}