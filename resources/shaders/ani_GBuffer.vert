#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

#extension GL_ARB_shading_language_include : require
#include </SkeletonAnimation.glsl>

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec4 pixelPosition;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform int offset;

void main()
{
    vec4 worldPos = M * animationTransForm(vec4(position, 1.0), gl_VertexID + offset);
    FragPos = worldPos.xyz;
    gl_Position = P * V * worldPos;
    TexCoords = texCoords;

    mat3 normalMatrix = transpose(inverse(mat3(M)));
    Normal = normalMatrix * normal;

    pixelPosition = gl_Position;
}