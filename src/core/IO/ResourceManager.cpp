#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model/Model.h"
#include "IO/ResourceManager.h"

namespace SPW
{
    glm::vec3 toVec3(const aiVector3D& _val)
    {
        return glm::vec3(_val.x, _val.y, _val.z);
    }

    glm::quat toQuat(const aiQuaternion& _val)
    {
        return glm::quat(glm::vec4(_val.x, _val.y, _val.z, _val.w));
    }

    glm::vec3 quatToEuler(const glm::quat& _quat)
    {
        return glm::eulerAngles(_quat);
    }

    // `mat` is an `aiMatrix4x4`
    glm::mat4 toMat4(const aiMatrix4x4& mat) {
        glm::mat4 result;
        result[0][0] = mat.a1; result[1][0] = mat.a2; result[2][0] = mat.a3; result[3][0] = mat.a4;
        result[0][1] = mat.b1; result[1][1] = mat.b2; result[2][1] = mat.b3; result[3][1] = mat.b4;
        result[0][2] = mat.c1; result[1][2] = mat.c2; result[2][2] = mat.c3; result[3][2] = mat.c4;
        result[0][3] = mat.d1; result[1][3] = mat.d2; result[2][3] = mat.d3; result[3][3] = mat.d4;
        return result;
    }

	namespace fs = std::filesystem;

	[[nodiscard]] std::shared_ptr<Mesh> ProcessMeshNode(aiMesh* mesh, const aiScene* scene)
	{
		std::shared_ptr<Mesh> tmp = std::make_shared<Mesh>();

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
		    // positions
		    glm::vec3 position{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			// normals
	        glm::vec3 normal = mesh->HasNormals()
	            ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
	            : glm::vec3();

			// texture coordinates
			bool uv_exist = mesh->mTextureCoords[0];
			glm::vec2 uv = uv_exist
				? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
				: glm::vec2();

			// tangents
			glm::vec3 tangent = uv_exist
				? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)
				: glm::vec3();

			// bitangents
			glm::vec3 bitangent = uv_exist
				? glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z)
				: glm::vec3();

			tmp->vertices.emplace_back(Vertex{position, normal, uv, tangent, bitangent});
		}

	    // Indices
	    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	    {
			const auto& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				tmp->indices.emplace_back(face.mIndices[j]);
	    }

	    // Bones
	    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			const aiBone* bone = mesh->mBones[i];

			// TODO Open Animation Branch to DO
			// Weights
			// std::vector<Weight> weights;
			// for (unsigned int j = 0; j < bone->mNumWeights; j++)
			// 	weights.emplace_back(Weight{ bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight });

			// m_BoneInfos.emplace_back(
			// 	std::make_shared<BoneInfo>(bone->mName.C_Str(), bone->mNumWeights, std::move(weights), toMat4(bone->mOffsetMatrix))
			// );
		}

		int numBones = mesh->mNumBones;

        // Iterate through each bone and get its parent bone index
        for (int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];
            int parentBoneIndex = -1;

            // Check if the bone has a parent node
            if (bone->mNode->mParent)
            {
                // Find the parent bone index by iterating through the bone array
                for (int j = 0; j < mesh->mNumBones; j++)
                {
                    if (i == j) continue;

                    aiBone* otherBone = mesh->mBones[j];
                    if (otherBone->mNode == bone->mNode->mParent) {
                        parentBoneIndex = j;
                        break;
                    }
                }
            }
            // m_BoneInfos[i]->parentID = parentBoneIndex;
        }

        // Iterate through each bone and get its children indices
        for (int i = 0; i < mesh->mNumBones; i++)
        {
        	// TODO Open Animation Branch to DO
            // const auto& parentID = m_BoneInfos[i]->parentID;
            // if (parentID == -1) continue;
            // m_BoneInfos[parentID]->childrenIDs.emplace_back(i);
        }

    	// TODO Deal with Materials

		return tmp;
	}

	std::vector<std::shared_ptr<Mesh>> ProcessNodes(aiNode* node, const aiScene* scene)
	{
		std::vector<std::shared_ptr<Mesh>> meshes;
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.emplace_back(std::move(ProcessMeshNode(mesh, scene)));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			auto res = ProcessNodes(node->mChildren[i], scene);
			meshes.insert(std::end(meshes), std::begin(res), std::end(res));
		}

		return meshes;
	}

	std::shared_ptr<Model> ResourceManager::LoadModel(const std::filesystem::path& _filePath)
	{
		const auto& fileExtension = _filePath.extension();
		// TODO TypeCheck

		// Create an instance of the Assimp importer
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_filePath.string().c_str()
				, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs 
				| aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return nullptr;
		}

		std::cout << "SUCESS::ASSIMP::" << _filePath << std::endl;
		
		std::shared_ptr<Model> model = std::make_shared<Model>(ProcessNodes(scene->mRootNode, scene));

		return model;

		// if (scene->HasAnimations()) ProcessAnimationClips(scene->mRootNode, scene);
	}
}