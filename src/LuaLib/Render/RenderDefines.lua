DepthCompUniform = {
    EQUAL_Type = 0,
    LEQUAL_Type = 1,
    LESS_Type = 2
}

MaterialUniform = {
    -- textures from files
    AlbedoType = 1 << 1,
    NormalType = 1 << 2,
    MetallicType = 1 << 3,
    RoughnessType = 1 << 4,
    AOType = 1 << 5,

    -- AlbedoConstant is vec3
    AlbedoConstantType = 1 << 6,
    -- NormalConstantType = 1 << 7,

    -- float
    MetallicConstantType = 1 << 8,
    RoughnessConstantType = 1 << 9,
    AOConstantType = 1 << 10,

    -- diffsuse, specular, ambient, and emissive as vec3
    DiffuseType = 1 << 11,
    SpecularType = 1 << 12,
    AmbientType = 1 << 13,
    EmissiveType = 1 << 14,

    -- transparent and refraction as vec3
    TransparentType = 1 << 15,
    RefractionType = 1 << 16,

    -- float
    EmissiveIntensityType = 1 << 17,
    SpecularPowerType = 1 << 18,
    TransparentIntensityType = 1 << 19,
    RefractionIntensityType = 1 << 20,

    -- cubemap from files
    CubeTextureType = 1 << 21,
}

TransformUniform = {
    -- camera related transform
    M = 1 << 1,
    V = 1 << 2,
    P = 1 << 3,
    MVP = 1 << 4,
    MV = 1 << 5,
    VP = 1 << 6,
}

ContextUniform = {
    -- frame size
    FrameWidth = 1 << 1,
    FrameHeight = 1 << 2,
    -- camera position
    CameraPosition = 1 << 3,
    -- random uniform float
    RandomNumber = 1 << 4,
}

LightUniform = {
    -- light input point light array
    PointLight = 1 << 1,
    -- light input directional light array
    DirectionalLight = 1 << 2,
    -- dirential light transform
    DLightTransform = 1 << 3,
    -- plight array size
    PLightArraySize = 1 << 4,
    -- dlight array size
    DLightArraySize = 1 << 5,
}

ClearType = {
    -- don't clear
    NoClear = 0,
    ClearColor = 1 << 1,
    ClearDepth = 1 << 2,
    ClearStencil = 1 << 3,
}

AttachmentType = {
    -- color
    ColorType = 1 << 1,

    -- cube map
    CubeMapType = 1 << 2,
}

ScreenAttachmentType = {
    -- unknown type
    UnknownScreenAttachmentType = 0,
    -- screen buffer
    ScreenColorType = 1 << 1,

    -- screen depth buffer
    ScreenDepthType = 1 << 2,
}

-- color attachment format
ColorAttachmentFormat = {
    -- 8 bit
    R8 = 1 << 1,
    RG8 = 1 << 2,
    RGB8 = 1 << 3,
    RGBA8 = 1 << 4,

    -- 16 bit
    R16 = 1 << 5,
    RG16 = 1 << 6,
    RGB16 = 1 << 7,
    RGBA16 = 1 << 8,

    -- 32 bit
    R32 = 1 << 9,
    RG32 = 1 << 10,
    RGB32 = 1 << 11,
    RGBA32 = 1 << 12,

    -- depth
    Depth = 1 << 13,
};

-- repeat info
RepeatType = {
    NoRepeat = 0,
    RepeatForDLights = 1 << 1,
    RepeatForPLights = 1 << 2,
    -- todo : add light probe repeat type
};

DefaultGraph = {
    PBRShadowGraph = "PBRShadowGraph",
    SkyBoxGraph = "SkyBoxGraph",
};

DefaultNode = {
    PointShadowNode = "PointShadowNode",
    DirectionalShadowNode = "DirectionalShadowNode",
    PBRShadowLightingNode = "PBRShadowLightingNode",
    SkyBoxNode = "SkyBoxNode",
};

DefaultShader = {
    PBRShadowShader = "PBRShadowShader",
    PBRShadowAniShader = "PBRShadowAniShader",
    PointShadowShader = "PointShadowShader",
    DirectionalShadowShader = "DirectionalShadowShader",
    PointShadowAniShader = "PointShadowAniShader",
    DirectionalShadowAniShader = "DirectionalShadowAniShader",
    SkyBoxShader = "SkyBoxShader",
};