#extension GL_ARB_shading_language_include : require

// return the scale color of BlinnPhong
// should multiply the base color at this point
vec3 BlinnPhong(vec3 normal, vec3 viewDir, vec3 lightDir, 
                vec3 diffColor, vec3 specColor, vec3 lamberColor,
                float diffusion, float lambertin, float shininess, int specularPower) 
{
    vec3 scale = lamberColor * lambertin;
    
    // calculate distance of light and position
    float distance = length(lightDir);

    lightDir = normalize(lightDir);
    distance = distance * distance;

    // diffusion color
    float NdotL = dot(normal, lightDir);
    float intensity = max(0, min(1, NdotL));
    scale += intensity * diffColor * diffusion / distance;

    // half vector between light and view
    vec3 halfVec = normalize(lightDir + normalize(viewDir));

    // specular light
	float NdotH = dot(normal, halfVec);
    intensity = pow(max(0, min(1, NdotH)), specularPower);
    scale += intensity * specColor * shininess / distance
    return scale
}