#version 420 core
#extension GL_ARB_shading_language_include : require
#include<D:/Project/shaowenSpprow/Sparrow/resources/shaders/structure.glsl>

out vec4 FragColor;
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;


uniform DLight DLights[10];
uniform PLight PLights[10];
//shader.SetUnifromValue<glm::vec4>("DLights[0].position",glm::vec4(1.0f));


in vec2 TexCoords;

void main()
{
    FragColor = texture(albedoMap,TexCoords);
}