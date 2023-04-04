#version 430 core
#extension GL_ARB_shading_language_include : require

#include </SkeletonAnimation.glsl>

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aCoords;
//layout(early_fragment_tests) in;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 lightSpaceMatrix[10];

out vec2 TexCoords;
out vec3 normal;
out vec4 position;
out vec4 FragPosLightSpace[10];

void main()
{
    vec4 position = vec4(aPos, 1.0);

    position = M * position;
    //position = M*animationTransForm(position, gl_VertexID);
    gl_Position = P * V * position;
    
    TexCoords = aCoords;
    normal = transpose(inverse(mat3(M))) * aNormal;
    for(int i = 0; i<10; i++)
    {
        FragPosLightSpace[i] = lightSpaceMatrix[i] * position;
    }
}