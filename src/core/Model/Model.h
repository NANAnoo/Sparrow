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
        Model(std::string const& path, std::shared_ptr<RenderBackEndI> renderBackEnd,bool gamma = false) : directory(path),gammaCorrection(gamma)
        {
            loadModel(path,renderBackEnd);
            //can not use virtual function in constractor;
        }

        void loadModel(const std::string& path,std::shared_ptr<RenderBackEndI> renderBackEnd);

        // draws the model, and thus all its meshes
        void Draw(std::shared_ptr<RenderBackEndI> renderBackEnd)
        {
            for (unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].Draw(renderBackEnd);
        }

    };
}
#endif //SPARROW_MODEL_H
