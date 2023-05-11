#version 430 core
#extension GL_ARB_shading_language_include : require

#include </structure.glsl>


uniform PLight light;

in vec4 position;

void main()
{
    gl_FragDepth = length(light.position - vec3(position)) / 100.f;
}