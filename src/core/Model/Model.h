//
// Created by sgsdi on 2023/2/19.
//

#ifndef SPARROW_MODEL_H
#define SPARROW_MODEL_H
#include "Mesh.h"
#include <string>
namespace SPW
{
    class Model
    {
    public:
        // model data
        std::vector<Mesh>  meshes;
        std::string directory;
        bool gammaCorrection;

        // constructor, expects a filepath to a 3D model.
        Model(std::string const& path, bool gamma = false) : directory(path),gammaCorrection(gamma)
        {
            //loadModel(path);
            //can not use virtual function in constractor;
        }

        virtual void loadModel(std::string& path) = 0;

        // draws the model, and thus all its meshes
        void Draw()
        {
            for (unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].Draw();
        }

    };
}
#endif //SPARROW_MODEL_H
