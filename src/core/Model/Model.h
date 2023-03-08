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
#include "Render/shader.h"
#include "iostream"

namespace SPW
{
//  struct KeyFrame;
//  struct AnimationClip;
//  struct AnimNode;
//  struct BoneInfo;
//  struct Weight;
//
//  struct Weight
//  {
//    uint32_t vertexID;
//    float value;
//  };
//
//  struct BoneInfo
//  {
//    std::string name;
//    uint32_t mNumWeights;
//    std::vector<Weight> weights;
//    glm::mat4 offsetMatrix;
//
//    BoneInfo(std::string&& _name, uint32_t _mNumWeights, std::vector<Weight>&& _weights, glm::mat4 _offsetMatrix)
//        :name(_name), mNumWeights(_mNumWeights), weights(_weights),offsetMatrix(_offsetMatrix)
//    {	}
//  };
//
//  struct AnimationClip
//  {
//    std::string name;
//    double duration;
//    uint32_t FPS;
//    std::vector<AnimNode> nodeAnimations;
//  };
//
//  struct AnimNode
//  {
//    std::string nodeName;
//    std::vector<KeyFrame> positionKeys;
//    std::vector<KeyFrame> rotationKeys;
//    std::vector<KeyFrame> scalingKeys;
//  };
//
//  struct KeyFrame
//  {
//    float time;
//    glm::vec3 value;
//  };

    class Model
    {
    public:
        Model() = default;
        explicit Model(std::vector<std::shared_ptr<Mesh>>&& _meshes) { m_Meshes = std::move(_meshes); }
    	  ~Model() {}

        [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }
        void AddMesh(std::shared_ptr<Mesh> _mesh) { m_Meshes.emplace_back(std::move(_mesh)); }

        // draws the model, and thus all its meshes
        void Draw(std::shared_ptr<RenderBackEndI> &renderBackEnd,  const ShaderHandle& handle)
        {
            for (auto & mesh : m_Meshes)
            {
                mesh->setShader(renderBackEnd, handle);
                mesh->Draw(renderBackEnd);
            }
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
