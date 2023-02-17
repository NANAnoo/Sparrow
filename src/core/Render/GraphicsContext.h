//
// Created by sgsdi on 2023/2/17.
//

#ifndef SPARROW_GRAPHICSCONTEXT_H
#define SPARROW_GRAPHICSCONTEXT_H

#include <memory>
namespace SPW
{
    class GraphicsContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static std::shared_ptr<GraphicsContext> Create(void* window);

    };
}



#endif //SPARROW_GRAPHICSCONTEXT_H