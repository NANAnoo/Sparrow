//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_RENDERAPII_H
#define SPARROW_RENDERAPII_H
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

    private:
        static RenderAPIType currentRenderAPI = RenderAPIType::OpenGL;

    };

}

#endif //SPARROW_RENDERAPII_H
