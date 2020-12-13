#version 330 core
out vec4 FragColor;

uniform samplerCube skybox;

in vec3 TexCoords;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}