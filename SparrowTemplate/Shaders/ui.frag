#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D background;

void main()
{
    FragColor = vec4(texture(background, TexCoords).rgb, 1.0);
}