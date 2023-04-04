#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 M;
uniform mat4 VP;

out vec4 position;

void main()
{
    position = M * vec4(aPos, 1.0);
    gl_Position = VP * position;
}