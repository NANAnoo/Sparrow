#version 410 core
#extension GL_ARB_shading_language_include : require

#include </structure.glsl>
#include </BlinnPhong.glsl>

out vec4 FragColor;
uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D normalMap;
uniform sampler2D AoMap;

// parameters of Blinn phong
uniform float diffusion;
uniform float shininess;
uniform float lambertin;
uniform int specularPower;

uniform vec3 camPos;

uniform DLight DLights[10];
uniform PLight PLights[10];
//shader.SetUnifromValue<glm::vec4>("DLights[0].position",glm::vec4(1.0f));


in vec2 TexCoords;
in vec3 normal;

void main()
{
    vec3 BP_scale;
    vec3 lightColor = PLights[0].intensity * PLights[0].color;
    // BP_scale = BlinnPhong(normal, PLights[0].position - gl_Position, camPos - gl_Position,
    //                         lightColor, lightColor, lightColor,
    //                         diffusion, lambertin, shininess, specularPower);
                            
    FragColor = texture(albedoMap, TexCoords);

}