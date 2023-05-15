#ifndef SSAO_GLSL
#define SSAO_GLSL

#define PI 3.141592653589793
#define PI2 6.283185307179586

int kernelSize = 32;

float radius = 0.5;

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 100.0f; // 投影矩阵的远平面
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

vec3 randomDirection(vec4 temp)
{
    float u = rand_2to1(temp.xy, 0);
    float v = rand_2to1(temp.zw, 0);

    float theta = PI2 * u;
    float phi = PI2 * v;

    float x = sin(phi) * cos(theta);
    float z = cos(phi) * cos(theta);
    float y = sin(theta);

    return vec3(x, y, z) * (rand_2to1(temp.xz, 0));
}

vec3 lerp(float a, float b, vec3 v) {
    return v * (b - a) + a;
}

float getAO(vec3 normal,vec3 worldPos,float Depth,mat4 view, mat4 proj)
{
    float occlusion = 0.0;
    for(int i = 0;i < kernelSize; i++)
    {
        float x = (i-1);
        float y = (i);
        float z = (i+1);
        float w = (i+2);
        vec3 randomVec3 = randomDirection(vec4(x,y,z,w));
        if(dot(randomVec3,normal)<0)
            randomVec3 = - randomVec3;
        vec3 samp = worldPos + lerp(0.1, 1.0, randomVec3) * radius;
        vec4 offset = proj * vec4(view * vec4(samp, 1.0f));
        vec3 screenPos = offset.xyz / offset.w; // [-1,1]
        screenPos = (screenPos + 1.0f) / 2.0f;
        float samp_depth = LinearizeDepth(screenPos.z);
        float sampleDepth = -texture(gPosition, screenPos.xy).a;

        occlusion += ((samp_depth - 0.00001 > sampleDepth && sampleDepth > 0) ? 1.0 : 0.0);
    }
    occlusion = occlusion/kernelSize;

    return 1.0 - occlusion;
}
#endif
