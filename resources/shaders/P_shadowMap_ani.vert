#version 430 core
layout (location = 0) in vec3 aPos;

#extension GL_ARB_shading_language_include : require

#include </SkeletonAnimation.glsl>

uniform mat4 M;
uniform mat4 VP;

uniform int offset;

out vec4 position;

void main()
{
    position = M * animationTransForm(vec4(aPos, 1.0), gl_VertexID + offset);
    gl_Position = VP * position;
}