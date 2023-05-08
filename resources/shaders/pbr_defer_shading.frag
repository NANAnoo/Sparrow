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

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform DLight DLights[10];
uniform int DLightCount;
uniform PLight PLights[10];
uniform int PLightCount;
uniform vec3 camPos;
uniform mat4 lightSpaceMatrix[10];

in vec2 TexCoords;
out vec4 FragColor;
vec4 FragPosLightSpace[10];


#define NUM_SAMPLES 15
#define NUM_RINGS 11
vec2 poissonDisk[NUM_SAMPLES];

vec3 normal = texture(gNormal,TexCoords).rgb;
vec3 position = texture(gPosition, TexCoords).rgb;
float roughness = texture(gMetalRognessAO, TexCoords).g;
vec3 VDir = normalize(camPos - position);
vec3 albedo     = texture(gAlbedo, TexCoords).rgb;
float metallic  = texture(gMetalRognessAO, TexCoords).r;

#include</shadow.glsl>

#include </SSAO.glsl>
#include </SSR.glsl>

vec3 PBR(vec3 N,vec3 position)
{
    vec3 lighting = vec3(0, 0, 0);

    float ao        = getAO(N,position,-texture(gPosition, TexCoords).w,V,P);
    for (int i = 0; i < PLightCount && i < 10; i ++) {
        lighting += PBR_P(albedo,metallic,roughness,N,VDir,position,camPos,PLights[i],0);
    }
    for (int i = 0; i < DLightCount && i < 10; i ++)
    {
        float shadow = 0.f;
        vec3 projCoords = FragPosLightSpace[i].xyz / FragPosLightSpace[i].w;
        if (abs(projCoords.x) < 1.0 && abs(projCoords.y) < 1)
        {
            projCoords = projCoords * 0.5 + vec3(0.5f,0.5f,0.5f);
            poissonDiskSamples(projCoords.xy);
            shadow = PCSS(projCoords,i);
        }
        lighting += PBR_D(albedo,metallic,roughness,N,VDir,vec3(position),camPos,DLights[i], shadow);
    }
    return (lighting + vec3(0.03) * albedo) * ao;
}

void main()
{

    for(int i = 0; i < 10 && i < DLightCount; i++)
    {
        FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(position,1.0f);
    }
    gl_FragDepth = texture(gDepth, TexCoords).r;
    vec3 PBRColor = PBR(normal,position);

    FragColor = vec4(PBRColor,1.0f);
}
