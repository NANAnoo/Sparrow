/*
 * Created by elecfrog@du
 * 2023/02/16
 * Stay Empty for API specification
 */

#pragma once

namespace SPW
{
    class Shader;

    class Texture;
    class Texture2D;
    class TextureCube;
    class TextureCube;
    class TextureDepth;
    class TextureDepthArray;

    class UniformBuffer;
    class Framebuffer;

    //... Wait to Add more

    // Definitions for Shaders
    enum class ShaderType : int
    {
            VERTEX,
            FRAGMENT,
            GEOMETRY,
            TESSELLATION_CONTROL,
            TESSELLATION_EVALUATION,
            COMPUTE,
            UNKNOWN
    };

    enum class CullMode
    {
        FRONT,
        BACK,
        FRONTANDBACK,
        NONE
    };

    enum class PolygonMode
    {
        FILL,
        LINE,
        POINT
    };

    // Definitions for Textures
    enum class TextureType
    {
        COLOR,
        DEPTH,
        DEPTHARRAY,
        CUBE,
        OTHER
    };

    enum class TextureWrap
    {
        NONE,
        REPEAT,
        CLAMP,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class TextureFilter
    {
        NONE,
        LINEAR,
        NEAREST
    };


    //... Wait to Add more
}
