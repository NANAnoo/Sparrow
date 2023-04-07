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
uniform samplerCubeArray P_shadowMap;
uniform sampler2DArray shadowMap;

uniform vec3 camPos;

uniform DLight DLights[10];
uniform int DLightCount;
uniform PLight PLights[10];
uniform int PLightCount;

in vec2 TexCoords;
in vec3 normal;
in vec4 position;
in vec4 FragPosLightSpace[10];

#define NUM_SAMPLES 11
#define NUM_RINGS 15
vec2 poissonDisk[NUM_SAMPLES];
#include</shadow.glsl>

float ShadowCalculation(int slot)
{
    // get vector between fragment position and light position
    vec3 fragToLight = vec3(position) - PLights[slot].position;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(P_shadowMap, vec4(fragToLight,float(slot))).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= 100.f;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.0001; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
} 

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
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao        = texture(AoMap, TexCoords).r;
    vec3 V = normalize(camPos - position.xyz);

    for (int i = 0; i < PLightCount && i < 10; i ++) {
        float shadow = ShadowCalculation(i);
        BP_scale += PBR_P(albedo,metallic,roughness,ao,N,V,vec3(position),camPos,PLights[i], shadow);
    }

    for (int i = 0; i < DLightCount && i < 10; i ++) {
        float shadow = 0.f;
        vec3 projCoords = FragPosLightSpace[i].xyz / FragPosLightSpace[i].w;
        if (abs(projCoords.x) < 1.0 && abs(projCoords.y) < 1) {
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
    vec3 norm = normalize(normal);
    vec3 N = getNormalFromMap();
    vec3 BP_scale =PBR(N);

    FragColor = vec4(BP_scale, 1.f);

}