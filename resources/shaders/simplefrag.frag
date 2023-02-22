#version 330 core
out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;

in vec2 TexCoords;

void main()
{
    FragColor = texture(albedoMap,TexCoords);
}