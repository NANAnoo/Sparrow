#ifndef SSR_GLSL
#define SSR_GLSL

mat4 dither = mat4(
        0,       0.5,    0.125,  0.625,
        0.75,    0.25,   0.875,  0.375,
        0.1875,  0.6875, 0.0625, 0.5625,
        0.9375,  0.4375, 0.8125, 0.3125
);

vec3 SSR()
{
    vec3 reflectDir = reflect(-VDir, normal);
    reflectDir = normalize(reflectDir);

    int sampleCoordX = int(mod((textureSize(gPosition,0).x * TexCoords.x),4));
    int sampleCoordY = int(mod((textureSize(gPosition,0).y * TexCoords.y),4));
    float offset = dither[sampleCoordX][sampleCoordY];

    float step = 0.001f;
//int stepNum = int(distance(farPos - viewPos) / step);
    int stepNum = 1000;

    vec3 SSRColor = vec3(0.0f,0.0f,0.0f);

    for(int i = 1;i <= stepNum;i++)
    {
        float delta = step * i + offset;
        vec4 ScreenPos = P*V*vec4((position + (reflectDir * delta)),1.0f);
        vec3 Pos = ScreenPos.xyz/ScreenPos.w;
        float d = Pos.z;
        Pos = Pos * 0.5 + vec3(0.5f,0.5f,0.5f);
        vec2 uv = Pos.xy;

        if(uv.x > 0 && uv.x < 1 && uv.y > 0 && uv.y < 1)
        {
            float depth = texture2D(gDepth, uv).r;
            if(d > depth)
            {
                if(abs(d - depth) < 0.000005)
                {
                    SSRColor = texture2D(gAlbedo, uv).xyz;
                }
                break; // 深度比较无论通不通过，都跳出查找反射颜色
            }
        }
    }
    return SSRColor;
}
#endif

