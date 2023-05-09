#define PI 3.141592653589793
#define PI2 6.283185307179586

 const float RandomSeed = 0.5;

highp float rand_2to1(vec2 uv ) {//传入一个二维数，传出一个假随机数。
    // 0 - 1
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot( uv.xy, vec2( a,b ) ) + RandomSeed;
    highp float sn = mod( dt, PI );
    return fract(sin(sn) * c);//只取小数部分（取值范围0~1，若为负+1）
}

void poissonDiskSamples(const in vec2 randomSeed)
{
    float ANGLE_STEP = PI2 * float(NUM_RINGS)/float( NUM_SAMPLES);//角位移大小
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES); //采样数的倒数

    float angle = rand_2to1(randomSeed) * PI2;//初始角度（弧度）
    float radius = INV_NUM_SAMPLES;//初始半径
    float radiusStep = radius;     //半径增量

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
        radius += radiusStep;//半径增加
        angle += ANGLE_STEP;//弧度增加
    }
}

float ShadowCalculation(vec3 projCoords, int slot)
{
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, vec3(projCoords.xy,float(slot))).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.005 * (1.0 - dot(normal, -DLights[slot].direction)), 0.005);
    //float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}
float PCF(vec3 projCoords,int r,int slot)
{
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, vec3(projCoords.xy,float(slot))).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = max(0.005 * (1.0 - dot(normal, -DLights[slot].direction)), 0.000001);

       //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;//每像素偏移距离
    poissonDiskSamples(projCoords.xy);
    for(int i=0;i<NUM_SAMPLES;i++)
    {
        vec2 coord;
        coord = projCoords.xy + vec2(r) * poissonDisk[i] * texelSize;
        coord.x = clamp(coord.x,0.0f,1.0f);
        coord.y = clamp(coord.y,0.0f,1.0f);

        float pcfDepth = texture(shadowMap, vec3(coord,float(slot))).r;
        shadow += float(int(currentDepth - bias  > pcfDepth));
    }

    shadow /= float(NUM_SAMPLES);
    if(projCoords.z >= 1.0||projCoords.z<=0.0||projCoords.x-0.1<=0||projCoords.y-0.1<=0)
        shadow = 0.0;

    return shadow;
}

float averageBlockDep(vec3 projCoords,vec2 texelSize,int slot)
{
    float blockerZ = 0.0;//遮挡物总深度
    int count = 0;
    float r=10;
    poissonDiskSamples(projCoords.xy+vec2(0.1314,0.351));
    float bias = max(0.005 * (1.0 - dot(normal, -DLights[slot].direction)), 0.005);
    for(int i=0;i<NUM_SAMPLES;++i)
    {
        vec2 coord = projCoords.xy + vec2(r) * poissonDisk[i] * texelSize;
        coord.x = clamp(coord.x,0.0f,1.0f);
        coord.y = clamp(coord.y,0.0f,1.0f);

        float depth = texture(shadowMap, vec3(coord,float(slot))).r;
        if (depth < projCoords.z-bias )
        {//如果为遮挡物
            count++;
            blockerZ += depth;
        }
    }

    if(count == 0)
        return 1.0f;
    return blockerZ / count;
}

float PCSS(vec3 projCoords,int slot)
{
    const float weightOfLight = 20.0;

    //每像素偏移距离
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;

    //PCSS核心算法
    float visibility = 0.0;
    //第一步计算平均遮挡物深度
    float averBlocker = averageBlockDep(projCoords,texelSize,slot);
    //return averBlocker;
    //第二步，计算半影半径
    float penumbra = max(projCoords.z - averBlocker,0.0) / averBlocker* weightOfLight ;
    //第三步 PCF
    visibility = PCF(projCoords,int(penumbra)+1,slot);

    return visibility;
}