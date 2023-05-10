#version 420 core
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D Screen;
uniform sampler2D gPosition;
uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gMetalRognessAO;

uniform mat4 V;
uniform mat4 P;

#include</SSR.glsl>

void main()
{
    vec3 color = SSR(texture(gMetalRognessAO, TexCoords).g,texture(gAlbedo, TexCoords).xyz,texture(gMetalRognessAO, TexCoords).r, Screen);
    FragColor = vec4(color, 1.0);
}