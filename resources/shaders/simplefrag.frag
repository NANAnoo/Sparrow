#version 410 core
#extension GL_ARB_shading_language_include : require

#include </structure.glsl>
#include </BlinnPhong.glsl>

out vec4 FragColor;
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;
uniform sampler2D shadowMap;

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
in vec4 FragPosLightSpace;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, DLights[0].direction)), 0.005);;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
            shadow = 0.0;

    return shadow;
}
float PCF(vec4 fragPosLightSpace)
{
    float radius = 0.3f;
    int PCF_RADIUS = 5;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float shadow = 0.0;
    float currentDepth = projCoords.z;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); // 0 级纹理相邻纹素的距离
    for(int x = -PCF_RADIUS; x <= PCF_RADIUS; ++x) {
        for(int y = -PCF_RADIUS; y <= PCF_RADIUS; ++y) {
            float shadowMapDepth = texture(shadowMap, projCoords.xy + radius*vec2(x, y) * texelSize).r;
            float BIAS = max(0.05 * (1.0 - dot(normal, DLights[0].direction)), 0.005);;
            shadow += currentDepth-BIAS > shadowMapDepth  ? 1.0 : 0.0;
        }
    }
    float total = (2*PCF_RADIUS+1);
    return shadow/(total*total);
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 BP_scale = vec3(0, 0, 0);

    for (int i = 0; i < PLightCount && i < 10; i ++) {
        BP_scale += BlinnPhong_P(norm, vec3(position), camPos, PLights[i],
                            diffusion, lambertin, shininess, specularPower);
    }       
    for (int i = 0; i < DLightCount && i < 10; i ++) {
        BP_scale += BlinnPhong_D(norm, vec3(position), camPos, DLights[i],
                            diffusion, lambertin, shininess, specularPower);
    }
    //float shadow = ShadowCalculation(FragPosLightSpace);
    float shadow = PCF(FragPosLightSpace);

    FragColor = (1.0-shadow)*(vec4(BP_scale, 1.f) * texture(albedoMap, TexCoords));
}