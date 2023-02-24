#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aCoords;
//layout(early_fragment_tests) in;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 TexCoords;
void main()
{
    gl_Position = P*V*M*vec4(aPos, 1.0);
    TexCoords = aCoords;
}