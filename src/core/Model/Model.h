//
// Created by sgsdi on 2023/2/14.
//

#ifndef SPARROW_MODEL_H
#define SPARROW_MODEL_H
#include "Mesh.h"

namespace SPW
{

    class Model
    {
    public:
        std::vector<Mesh> meshes;

    public:
        void draw()
        {
            for(Mesh& m:meshes) m.draw();
        }
    };

}
#endif //SPARROW_MODEL_H
