#version 430 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gMetalRognessAO;

#include </structure.glsl>
#include </PBR.glsl>

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos.xyz);
    vec3 Q2  = dFdy(FragPos.xyz);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


void main()
{
    gPosition = FragPos;

    gNormal = getNormalFromMap();

    gAlbedo.rgb = texture(albedoMap, TexCoords).rgb;

    gMetalRognessAO.r = texture(metallicMap, TexCoords).r;
    gMetalRognessAO.g = texture(roughnessMap, TexCoords).g;
    gMetalRognessAO.b = texture(AoMap, TexCoords).b;
}