//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_RENDERAPII_H
#define SPARROW_RENDERAPII_H

#include <memory>
#include <glm/glm.hpp>
#include <IO/AttriVertex.h>
#include <vector>
namespace SPW
{


    enum class DepthComp
    {
        EQUAL,
        LEQUAL,
        LESS
    };

    class RenderAPII
    {
        enum class RenderAPIType
        {
            None = 0,
            OpenGL,
        };
    public:
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void DrawElement(std::vector<AttribVertex> vertices,std::vector<int> indices) = 0;
        virtual void DrawArray(std::vector<AttribVertex> vertices,int count) = 0;
        //depth
        virtual void DepthTest(bool enable) = 0;
        virtual void DepthMask(bool maskFlag) = 0;
        virtual void DepthFunc(DepthComp comp) = 0;

        //cull
        virtual void Cull(int32_t Bit) = 0;
        virtual void CullFrontOrBack(bool bFront) = 0;

    public:
        virtual void Init() = 0;
        static RenderAPIType getCurrent() { return currentRenderAPI;}
        virtual ~RenderAPII() = default;
    private:
        static inline RenderAPIType currentRenderAPI = RenderAPIType::OpenGL;

    };

}

#endif //SPARROW_RENDERAPII_H
