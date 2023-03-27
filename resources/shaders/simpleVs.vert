#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aCoords;
//layout(early_fragment_tests) in;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 lightSpaceMatrix;

out vec2 TexCoords;
out vec3 normal;
out vec4 position;
out vec4 FragPosLightSpace;

void main()
{
    position = M*vec4(aPos, 1.0);
    gl_Position = P*V*position;
    
    TexCoords = aCoords;
    normal = transpose(inverse(mat3(M))) * aNormal;
    FragPosLightSpace = lightSpaceMatrix * position;
}