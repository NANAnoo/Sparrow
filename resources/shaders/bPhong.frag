#version 430 core
#extension GL_ARB_shading_language_include : require

#include </structure.glsl>


out vec4 FragColor;
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;

uniform sampler2DArray shadowMap;

// parameters of Blinn phong
uniform float diffusion;
uniform float shininess;
uniform float lambertin;
uniform float specularPower;

uniform vec3 camPos;

uniform DLight DLights[10];
uniform int DLightCount;
uniform PLight PLights[10];
uniform int PLightCount;

in vec2 TexCoords;
in vec3 normal;
in vec4 position;
in vec4 FragPosLightSpace[10];

#include </BlinnPhong.glsl>

void main()
{
    vec3 norm = normalize(normal);
    vec3 BP_scale = vec3(0, 0, 0);

    for (int i = 0; i < PLightCount && i < 10; i ++) {
        BP_scale += BlinnPhong_P(norm, vec3(position), camPos, PLights[i],
                            diffusion, lambertin, shininess, specularPower);
    }       
    for (int i = 0; i < DLightCount && i < 10; i ++) {
        vec3 tempColor = BlinnPhong_D(norm, vec3(position), camPos, DLights[i],
                            diffusion, lambertin, shininess, specularPower,0.f);

        BP_scale += tempColor;
    }


    FragColor = (vec4(BP_scale, 1.f) * texture(albedoMap, TexCoords));
}