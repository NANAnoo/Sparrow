#version 410 core

uniform sampler2D gDepth;

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
    gl_FragDepth = texture(gDepth, TexCoords).r;
    FragColor = vec4(0, 0, 0, 1.f);
}
