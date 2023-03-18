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

out vec2 TexCoords;
out vec3 normal;
out vec4 position;
void main()
{
    position = vec4(aPos, 1.0);
    position = M*animationTransForm(position, gl_VertexID);
    gl_Position = P*V*position;
    
    TexCoords = aCoords;
    normal = mat3(transpose(inverse(M))) * aNormal;
}