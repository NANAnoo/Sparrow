#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aCoords;
layout(early_fragment_tests) in;

#extension GL_ARB_shading_language_include : require

#include </SkeletonAnimation.glsl>

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 lightSpaceMatrix[10];

uniform int offset;
uniform int DLightCount;

out vec2 TexCoords;
out vec3 normal;
out vec4 position;
out vec4 FragPosLightSpace[10];

void main()
{
    mat4 anim = animationTransFormMat(gl_VertexID + offset);
    position = M * anim * vec4(aPos, 1.0);
    gl_Position = P*V*position;
    
    TexCoords = aCoords;
    normal = transpose(inverse(mat3(M))) * aNormal;
    for(int i = 0; i < 10 && i < DLightCount; i++)
    {
        FragPosLightSpace[i] = lightSpaceMatrix[i] * position;
    }
}