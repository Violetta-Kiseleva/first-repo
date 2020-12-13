#version 330 core
out vec4 FragColor;

uniform sampler2D current_texture;

in vec2 TexCoords;

void main()
{    
    FragColor = texture(current_texture, TexCoords);
}