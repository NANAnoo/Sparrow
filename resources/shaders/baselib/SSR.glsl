#ifndef SSR_GLSL
#define SSR_GLSL

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 hash(vec3 vec)
{
    vec = fract(vec * Scale);
    vec += dot(vec, vec.yxz + K);
    return fract((vec.xxy + vec.yxx) * vec.zyx) * 2 - 1;
}

vec3 fresnelSchlickSSR(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float eps = 0.001;

bool rayIsOutofScreen(vec3 ray)
{
    return (ray.x > 1 || ray.y > 1 || ray.x < 0 || ray.y < 0||ray.z>1.0f||ray.z<0) ? true : false;
}

vec3 TraceRay(vec3 rayPos, vec3 dir, sampler2D Screen, int iterationCount)
{
    float sampleDepth;
    vec3 hitColor = vec3(0);
    bool hit = false;

    vec3 lastPos = rayPos;
    for(int i = 0; i < iterationCount; i++)
    {
        rayPos = rayPos + dir;

        if(rayIsOutofScreen(rayPos))
        {
            return vec3(0,0,0);
            break;
        }

        sampleDepth = texture(gDepth, rayPos.xy).r;
        float depthDif = rayPos.z - sampleDepth;
        if(depthDif >= 0)
        { //we have a hit
            if(depthDif < eps)
            {
                hit = true;
                hitColor = texture(Screen, rayPos.xy).rgb;
                break;
            }


        }
    }
    return hitColor;
}

vec3 SSR(float roughness,vec3 albedo,float metallic, sampler2D Screen)
{
    if(roughness>0.3) return vec3(0);

    vec3 outColor = vec3(0.0);

    const int SCR_WIDTH = textureSize(Screen,0).x;
    const int SCR_HEIGHT = textureSize(Screen,0).y;
    float maxRayDistance = 100.0f;

    //View Space ray calculation
    vec3 pixelPositionTexture;
    pixelPositionTexture.xy = TexCoords;
    vec3 normalView = normalize(vec3(V * vec4(texture(gNormal, pixelPositionTexture.xy).rgb,0.0f)));
    float pixelDepth = texture(gDepth, pixelPositionTexture.xy).r;	// 0< <1
    pixelPositionTexture.z = pixelDepth;

    vec4 positionView = inverse(P) * vec4(pixelPositionTexture.xy * 2 - vec2(1), pixelPositionTexture.z,1);
    vec4 wp = inverse(V) * positionView;
    positionView /= positionView.w;
    wp /= wp.w;
    vec3 reflectionView = normalize(reflect(positionView.xyz, normalView));

    int count = 1;
    float scale = float(count);
    while(count -- > 0) {
        vec3 jitter = mix(vec3(0), hash(wp.xyz), roughness * 0.5);

        reflectionView = normalize(reflectionView + jitter);

        if(reflectionView.z>0)
        {
           continue;
        }
        vec3 rayEndPositionView = positionView.xyz + reflectionView * maxRayDistance;
        eps = 0.0005;
        eps *= 1.0 - min(0.999, 1.5 * abs(dot(reflectionView,normalView)));

        //Texture Space ray calculation
        vec4 rayEndPositionTexture = P * vec4(rayEndPositionView,1);
        rayEndPositionTexture /= rayEndPositionTexture.w;
        rayEndPositionTexture.xyz = vec3(rayEndPositionTexture.xy*0.5f,rayEndPositionTexture.z)+vec3(0.5f,0.5f, 0);

        vec3 rayDirectionTexture = rayEndPositionTexture.xyz - pixelPositionTexture;

        ivec2 screenSpaceStartPosition = ivec2(pixelPositionTexture.x * SCR_WIDTH, pixelPositionTexture.y * SCR_HEIGHT);
        ivec2 screenSpaceEndPosition = ivec2(rayEndPositionTexture.x * SCR_WIDTH, rayEndPositionTexture.y * SCR_HEIGHT);
        ivec2 screenSpaceDistance = screenSpaceEndPosition - screenSpaceStartPosition;
        int screenSpaceMaxDistance = max(abs(screenSpaceDistance.x), abs(screenSpaceDistance.y)) / 2;
        rayDirectionTexture /= max(screenSpaceMaxDistance, 0.001f);
        outColor += TraceRay(pixelPositionTexture, rayDirectionTexture, Screen, screenSpaceMaxDistance);
    }
    //trace the ray

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);
    vec3 Fresnel = fresnelSchlickSSR(max(dot(normalize(normalView), normalize(positionView.xyz)), 0.0), F0);

    return outColor * Fresnel / scale;
}
/*
vec3 SSR()
{
    vec3 Screen0 = vec3(0,0,0);
    Screen0.xy = TexCoords;
    Screen0.z = texture(gDepth,TexCoords).r;

    vec3 reflectDir = normalize(reflect(-VDir, normal));

    vec4 temp = P*V*vec4(position + reflectDir,1.0f);
    vec3 Screen1 = vec3(temp.xy*0.5f,temp.z)/temp.w +vec3(0.5f,0.5f, 0);

    vec3 StepDir_Screen = normalize(Screen1 - Screen0);
    //Screen0.x = Screen0.x * textureSize(gPosition,0).x;
    //Screen0.y = Screen0.x * textureSize(gPosition,0).y;

    ivec2 screenSpaceStartPosition = ivec2(Screen0.x * 1280, Screen0.y * 726);
    ivec2 screenSpaceEndPosition = ivec2(Screen1.x * 1280, Screen1.y * 726);
    ivec2 screenSpaceDistance = screenSpaceEndPosition - screenSpaceStartPosition;
    int screenSpaceMaxDistance = max(abs(screenSpaceDistance.x), abs(screenSpaceDistance.y)) / 2;
    StepDir_Screen /= max(screenSpaceMaxDistance, 0.001f);

    vec3 outColor = TraceRay(Screen0, StepDir_Screen, screenSpaceMaxDistance);
    return outColor;

    /*
    for (int i = 1; i < 10000; ++i) {
        vec3 result = Screen0 + (StepDir_Screen * 1.0 / 5000.0f) * float(i);
        if (result.z < 0.0f || result.z > 1.0f || result.x < 0.0f || result.x > 1.0f || result.y < 0.0f ||
            result.y > 1.0f)
            return vec3(0, 0, 0);

        float depth = texture2D(gDepth, result.xy).r;

        if (result.z > depth) {
            if (abs(result.z - depth) < 0.000005) {
                return texture2D(gAlbedo, result.xy).xyz;
            }
            break; // 深度比较无论通不通过，都跳出查找反射颜色
        }

    }
    */
//}
#endif

