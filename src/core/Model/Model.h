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
    struct KeyFrame;
    struct AnimationClip;
    struct AnimationNode;
    struct BoneInfo;
    struct Weight;

    struct Weight
    {
        uint32_t vertexID;
        float value;
    };

    struct BoneInfo
    {
        uint32_t boneID;
        int32_t parentID{ -1 };
        std::vector<uint32_t> childrenIDs = std::vector<uint32_t>{};

        std::string name;
        uint32_t mNumWeights;
        std::vector<Weight> weights;
        glm::mat4 offsetMatrix;

        BoneInfo(std::string&& _name, uint32_t _mNumWeights, std::vector<Weight>&& _weights, glm::mat4 _offsetMatrix)
            :name(_name), mNumWeights(_mNumWeights), weights(_weights), offsetMatrix(_offsetMatrix)
        {	}
    };

    struct AnimationClip
    {
        std::string name;
        double duration;
        uint32_t FPS;
        uint32_t frameCount;
        std::vector<AnimationNode> nodeAnimations;
    };

    struct AnimationNode
    {
        std::string nodeName;
        std::vector<KeyFrame> keyFrames;
    };

    struct KeyFrame
    {
        double time;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 sacling;
    };

    class Model
    {
    public:
        Model() = default;
        explicit Model(const std::filesystem::path& _filePath, bool gamma = false);
	    Model(std::vector<std::shared_ptr<Mesh>>&& _meshes)
	    {
            m_Meshes = std::move(_meshes);
	    }

    	~Model() {
            std::cout << " ";
        };

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
