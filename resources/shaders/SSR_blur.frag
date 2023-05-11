#version 420 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D Screen;
uniform sampler2D reflection;
uniform sampler2D gDepth;

void main()
{
    vec2 tex_size = textureSize(Screen, 0);
    vec2 pixel_size = 1.0 / tex_size;
    vec2 offsets[9] = vec2[](
        vec2(-1.0,  1.0),
        vec2( 0.0f,    1.0),
        vec2( 1.0,  1.0),
        vec2(-1.0,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( 1.0,  0.0f),
        vec2(-1.0, -1.0),
        vec2( 0.0f,   -1.0),
        vec2( 1.0, -1.0)
    );

    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    vec3 reflect_color = vec3(0);
    for(int i = 0; i < 9; i++)
    {
        reflect_color += vec3(texture(reflection, TexCoords.st + pixel_size * offsets[i])) * kernel[i];
    }
    vec3 color = texture(Screen, TexCoords).rgb + reflect_color;
    color = color / (color + 1.0);
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
    gl_FragDepth = texture(gDepth, TexCoords).r;
}