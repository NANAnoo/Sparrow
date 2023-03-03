#extension GL_ARB_shading_language_include : require
struct DLight
{
    vec3 color;
    vec3 direction;
    float intensity;
};
struct PLight
{
    vec4 position;
    vec3 color;
    float intensity;
};