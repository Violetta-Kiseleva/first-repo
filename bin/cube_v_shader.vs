#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 TexCoords_1;
                
out vec2 TexCoords;
out vec3 FragPos;	// позиция текущего фрагмента
out vec3 Normal;
           
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;     
         
void main()
{                   
    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = TexCoords_1;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);		
} 