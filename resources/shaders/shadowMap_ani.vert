#version 430 core
layout (location = 0) in vec3 aPos;

#extension GL_ARB_shading_language_include : require

#include </SkeletonAnimation.glsl>

uniform mat4 lightSpaceMatrix;
uniform mat4 M;
uniform int offset;

void main()
{
    gl_Position = lightSpaceMatrix * M * animationTransForm(vec4(aPos, 1.0), gl_VertexID + offset);
}