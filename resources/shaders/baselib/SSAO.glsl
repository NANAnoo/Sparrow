#ifndef SSAO_GLSL
#define SSAO_GLSL

#define PI 3.141592653589793
#define PI2 6.283185307179586

int kernelSize = 32;


vec3 randomDirection(vec4 temp)
{
    float u = rand_2to1(temp.xy);
    float v = rand_2to1(temp.zw);

    float theta = 2.0 * 3.14159265359 * u;
    float phi = acos(2.0 * v - 1.0);

    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    return vec3(x, y, z) * (rand_2to1(temp.xz) * 0.5  + 0.5) * 0.01;
}

float getAO(vec3 normal,vec3 worldPos,float Depth,mat4 view, mat4 Proj)
{
    float occlusion = 0.0;
    for(int i = 0;i < kernelSize; i++)
    {
        float x = RandomSeed*(i-1);
        float y = RandomSeed*(i);
        float z = RandomSeed*(i+1);
        float w = RandomSeed*(i+2);
        vec3 randomVec3 = randomDirection(vec4(x,y,z,w));
        if(dot(randomVec3,normal)<0)
            randomVec3 = - randomVec3;
        vec3 pos = worldPos + randomVec3;

        vec4 tmp = view * vec4(pos, 1.0f);
        tmp = Proj * tmp;
        vec3 screenPos = tmp.xyz / tmp.w; // [-1,1]
        vec2 uv = (screenPos.xy + 1.0f) / 2.0f;

        float sampleDepth = texture(gPosition, uv).a;

        occlusion += Depth - 0.01 <sampleDepth ? 0.0 : 1.0;
    }
    occlusion = occlusion/kernelSize;

    return 1.0 - occlusion;
}

#endif