#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec4 pixelPosition;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
    vec4 worldPos = M * vec4(position, 1.0f);
    FragPos = worldPos.xyz;
    gl_Position = P * V * worldPos;
    TexCoords = texCoords;

    mat3 normalMatrix = transpose(inverse(mat3(M)));
    Normal = normalMatrix * normal;

    pixelPosition = gl_Position;
}