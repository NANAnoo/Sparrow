#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D background;

void main()
{
    vec4 color = texture(background, TexCoords);
    if (color.a < 0.5)
        discard;
    FragColor = vec4(color.rgb, 1.0);
}