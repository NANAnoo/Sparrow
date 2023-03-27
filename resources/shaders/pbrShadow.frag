#version 410 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include </structure.glsl>
#include </PBR.glsl>

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

#define NUM_SAMPLES 100
#define NUM_RINGS 15
vec2 poissonDisk[NUM_SAMPLES];
#include</shadow.glsl>

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(position.xyz);
    vec3 Q2  = dFdy(position.xyz);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 PBR(vec3 N){
    vec3 BP_scale = vec3(0, 0, 0);

    vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).g;
    float ao        = texture(AoMap, TexCoords).r;
    vec3 V = normalize(camPos - position.xyz);

    for (int i = 0; i < PLightCount && i < 10; i ++) {
        BP_scale += PBR_P(albedo,metallic,roughness,ao,N,V,vec3(position),camPos,PLights[i]);
    }

    for (int i = 0; i < DLightCount && i < 10; i ++) {
        vec3 projCoords = FragPosLightSpace[i].xyz / FragPosLightSpace[i].w;
        projCoords = projCoords * 0.5 + 0.5;
        poissonDiskSamples(projCoords.xy);
        float shadow = PCSS(projCoords,i);
        BP_scale += (1 - shadow) * PBR_D(albedo,metallic,roughness,ao,N,V,vec3(position),camPos,DLights[i]);
    }

    return BP_scale;
}
void main()
{
    vec3 norm = normalize(normal);
    vec3 N = getNormalFromMap();
    vec3 BP_scale =PBR(N);

    FragColor = vec4(pow(BP_scale, vec3(0.8)), 1.f);

}