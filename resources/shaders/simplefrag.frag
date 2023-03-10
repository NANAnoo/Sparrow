#version 410 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include </structure.glsl>
#include </BlinnPhong.glsl>
#include </PBR.glsl>

out vec4 FragColor;
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;

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

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(worldPos);
    vec3 Q2  = dFdy(worldPos);
    vec2 st1 = dFdx(texCoords);
    vec2 st2 = dFdy(texCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 BlinnPhong(vec3 norm){
    vec3 BP_scale = vec3(0, 0, 0);

    for (int i = 0; i < PLightCount && i < 10; i ++) {
            BP_scale += BlinnPhong_P(norm, vec3(position), camPos, PLights[i],
                                diffusion, lambertin, shininess, specularPower);
    }

    for (int i = 0; i < DLightCount && i < 10; i ++) {
            BP_scale += BlinnPhong_D(norm, vec3(position), camPos, DLights[i],
                                diffusion, lambertin, shininess, specularPower);
    }

    return BP_scale;
}

vec3 PBR(vec3 N){
    vec3 BP_scale = vec3(0, 0, 0);

    vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao        = texture(AoMap, TexCoords).r;
    vec3 V = normalize(camPos - position);

    for (int i = 0; i < PLightCount && i < 10; i ++) {
        BP_scale += PBR_P(albedo,metallic,roughness,ao,N,V,vec3(position),camPos,PLights[i]);
    }

    for (int i = 0; i < DLightCount && i < 10; i ++) {
        BP_scale += PBR_D(albedo,metallic,roughness,ao,N,V,vec3(position),camPos,DLights[i]);
    }

    return BP_scale;
}
void main()
{
    vec3 norm = normalize(normal);
    if (normalMap == (sampler2D)0) {
        vec3 N=norm;
    }
    else{
        vec3 N = getNormalFromMap();
    }
    //vec3 BP_scale =BlinnPhong(norm)
    vec3 BP_scale =PBR(N)

    FragColor = vec4(BP_scale, 1.f);

}