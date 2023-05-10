#version 430 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gMetalRognessAO;

#include </structure.glsl>
#include </PBR.glsl>

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 pixelPosition;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 100.0f; // 投影矩阵的远平面
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

vec2 tiledTexCoords(int level) {
    vec2 texCoords = TexCoords;
    texCoords.x *= level;
    texCoords.y *= level;
    float x = floor(texCoords.x);
    float y = floor(texCoords.y);
    texCoords.x -= x;
    texCoords.y -= y;
    return texCoords;
}

vec3 getNormalFromMap()
{
    vec2 tiled_texcoord = tiledTexCoords(100);
    vec3 tangentNormal = texture(normalMap, tiled_texcoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos.xyz);
    vec3 Q2  = dFdy(FragPos.xyz);
    vec2 st1 = dFdx(tiled_texcoord);
    vec2 st2 = dFdy(tiled_texcoord);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    gPosition.xyz = FragPos;
    gPosition.w = -LinearizeDepth(gl_FragCoord.z);
    vec2 tiled_texcoord = tiledTexCoords(100);
    if(length(texture(normalMap, tiled_texcoord).xyz)>0.0f)
        gNormal = getNormalFromMap();
    else
        gNormal = normalize(Normal);

    gAlbedo.rgb = texture(albedoMap, tiled_texcoord).rgb;

    gMetalRognessAO.r = texture(metallicMap, tiled_texcoord).r;
    gMetalRognessAO.g = texture(roughnessMap, tiled_texcoord).g;
    gMetalRognessAO.b = texture(AoMap, tiled_texcoord).b;
}