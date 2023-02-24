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
        explicit Model(const std::filesystem::path& _filePath, bool gamma = false);
	    Model(std::vector<std::shared_ptr<Mesh>>&& _meshes)
	    {
            m_Meshes = std::move(_meshes);
	    }

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

    private:
        // TODO: Add PrimitiveType type
        std::filesystem::path m_FilePath;
        std::filesystem::path m_Directory;
        std::vector<std::shared_ptr<Mesh>>  m_Meshes;
        std::vector<std::shared_ptr<AnimationClip>>  m_AnimationClips;
        std::vector<std::shared_ptr<BoneInfo>>  m_BoneInfos;
        bool m_GammaCorrection;
    };
}
#endif //SPARROW_MODEL_H
