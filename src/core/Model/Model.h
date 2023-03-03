//
// Created by sgsdi on 2023/2/19.
//

#ifndef SPARROW_MODEL_H
#define SPARROW_MODEL_H
#include <string>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model/Mesh.h"
#include "Render/RenderBackEndI.h"
#include "iostream"

namespace SPW
{
    class Model
    {
    public:
        Model() = default;
        explicit Model(std::vector<std::shared_ptr<Mesh>>&& _meshes) { m_Meshes = std::move(_meshes); }
    	~Model() {}

        [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }
        void AddMesh(std::shared_ptr<Mesh> _mesh) { m_Meshes.emplace_back(std::move(_mesh)); }

        // draws the model, and thus all its meshes
        void Draw(std::shared_ptr<RenderBackEndI> &renderBackEnd)
        {
            for (auto & mesh : m_Meshes)
                mesh->Draw(renderBackEnd);
        }

        void setUpModel(std::shared_ptr<RenderBackEndI> &renderBackEnd) {
            for (auto& mesh : m_Meshes)
                mesh->setupMesh(renderBackEnd);
        }

        void SetFilePath(const std::filesystem::path filePath)
        {
            m_FilePath = filePath;
            m_Directory = filePath.parent_path();
        }

    private:
        std::filesystem::path m_FilePath;
        std::filesystem::path m_Directory;
        std::vector<std::shared_ptr<Mesh>>  m_Meshes;
    };
}
#endif //SPARROW_MODEL_H
