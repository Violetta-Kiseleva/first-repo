#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
            
uniform vec3 cameraPos;
uniform samplerCube skybox;
         
void main()
{   
    // вектор направления камеры          
    vec3 I = normalize(Position - cameraPos);
          
    // вектор отражения 
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}