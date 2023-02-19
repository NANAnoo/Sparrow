//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_RENDERAPII_H
#define SPARROW_RENDERAPII_H

#include <memory>
namespace SPW
{
    class RenderAPII
    {
    public:
        enum class RenderAPIType
        {
            None = 0,
            OpenGL,
        };

    public:
        virtual void Init() = 0;
        static RenderAPIType getCurrent() { return currentRenderAPI;}
        virtual ~RenderAPII() = default;

    private:
        static inline RenderAPIType currentRenderAPI = RenderAPIType::OpenGL;

    };

}

#endif //SPARROW_RENDERAPII_H
