//
// Created by sgsdi on 2023/2/19.
//

#ifndef SPARROW_MODEL_H
#define SPARROW_MODEL_H
#include "Mesh.h"
#include <string>
#include "Render/RenderBackEndI.h"
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
        Model(std::string path, bool gamma = false) :
            directory(std::move(path)), gammaCorrection(gamma)
        {
            // loadModel(path,renderBackEnd);
        }

        // draws the model, and thus all its meshes
        void Draw(std::shared_ptr<RenderBackEndI> &renderBackEnd)
        {
            for (auto & mesh : meshes)
                mesh.Draw(renderBackEnd);
        }

        void setUpModel(std::shared_ptr<RenderBackEndI> &renderBackEnd) {
            for (auto & mesh : meshes)
                mesh.setupMesh(renderBackEnd);
        }
    };
}
#endif //SPARROW_MODEL_H
