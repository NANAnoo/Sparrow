#version 410 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include </PBR.glsl>
uniform sampler2D gDepth;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalRognessAO;
uniform sampler2DArray shadowMap;

uniform DLight DLights[10];
uniform int DLightCount;
uniform PLight PLights[10];
uniform int PLightCount;
uniform vec3 camPos;
uniform mat4 lightSpaceMatrix[10];

in vec2 TexCoords;
out vec4 FragColor;
vec4 FragPosLightSpace[10];
vec3 normal;

#define NUM_SAMPLES 11
#define NUM_RINGS 15
vec2 poissonDisk[NUM_SAMPLES];
#include</shadow.glsl>

vec3 PBR(vec3 N,vec3 position)
{
    vec3 BP_scale = vec3(0, 0, 0);

    vec3 albedo     = pow(texture(gAlbedo, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(gMetalRognessAO, TexCoords).r;
    float roughness = texture(gMetalRognessAO, TexCoords).g;
    float ao        = texture(gMetalRognessAO, TexCoords).b;
    vec3 V = normalize(camPos - position);

    for (int i = 0; i < PLightCount && i < 10; i ++) {
        BP_scale += PBR_P(albedo,metallic,roughness,ao,N,V,position,camPos,PLights[i],0);
    }
    for (int i = 0; i < DLightCount && i < 10; i ++)
    {
        float shadow = 0.f;
        vec3 projCoords = FragPosLightSpace[i].xyz / FragPosLightSpace[i].w;
        if (abs(projCoords.x) < 1.0 && abs(projCoords.y) < 1)
        {
            projCoords = projCoords * 0.5 + 0.5;
            poissonDiskSamples(projCoords.xy);
            shadow = PCSS(projCoords,i);
        }
        BP_scale += PBR_D(albedo,metallic,roughness,ao,N,V,vec3(position),camPos,DLights[i], shadow);
    }
    return BP_scale;
}
void main()
{
    vec3 position = texture(gPosition, TexCoords).rgb;
    for(int i = 0; i < 10 && i < DLightCount; i++)
    {
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(position,1.0f);
    }
    gl_FragDepth = texture(gDepth, TexCoords).r;
    vec3 Normal = texture(gNormal,TexCoords).rgb;
    normal = normalize(Normal);
    FragColor = vec4(PBR(Normal,position),1.0f);
}
