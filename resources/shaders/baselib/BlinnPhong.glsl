// return the scale color of BlinnPhong
// should multiply the base color at this point
#include </structure.glsl>

// BlinnPhong for Plight
vec3 BlinnPhong_P(vec3 normal, vec3 pos, vec3 camPos, PLight light,float ambient, float diffusion , float shininess, float specularPower) 
{
    vec3 scale = light.ambient * light.constant * ambient;
    
    // calculate distance of light and position
    vec3 lightDir = light.position - pos;
    float distance = length(lightDir);

    lightDir = normalize(lightDir);
    distance = distance * distance;

    // diffusion color
    float NdotL = dot(normal, lightDir);
    float intensity = max(0, min(1, NdotL));
    scale += intensity * light.diffuse * light.quadratic * diffusion / distance;

    // half vector between light and view
    vec3 viewDir = camPos - pos;
    vec3 halfVec = normalize(lightDir + normalize(viewDir));

    // specular light
	float NdotH = dot(normal, halfVec);
    intensity = pow(max(0, min(1, NdotH)), specularPower);
    scale += intensity * light.specular * light.quadratic * shininess / distance;
    return scale;
}

// BlinnPhong for Dlight
vec3 BlinnPhong_D(vec3 normal, vec3 pos, vec3 camPos, DLight light, float ambient, float diffusion , float shininess, float specularPower, float shadow)
{
    vec3 amb = light.ambient * ambient;
    
    // calculate distance of light and position
    vec3 lightDir = -normalize(light.direction);

    // diffusion color
    float NdotL = dot(normal, lightDir);
    float intensity = max(0, min(1, NdotL));
    vec3 dif = intensity * light.diffuse * diffusion;

    // half vector between light and view
    vec3 viewDir = camPos - pos;
    vec3 halfVec = normalize(lightDir + normalize(viewDir));

    // specular light
	float NdotH = dot(normal, halfVec);
    intensity = pow(max(0, min(1, NdotH)), specularPower);
    vec3 spe = intensity * light.specular * shininess;

    return (amb +(1.0 - shadow)*(dif + spe));
}