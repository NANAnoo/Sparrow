#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model/Model.h"
#include "Model/Animation/Skeleton.h"
#include "IO/FileSystem.h"
#include "IO/ResourceManager.h"
#include "Render/Material.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/include/stb_image.h>

namespace SPW
{
    glm::vec3 toVec3(const aiVector3D& _val)
    {
        return glm::vec3(_val.x, _val.y, _val.z);
    }

	glm::vec4 toVec4(const aiColor4D& _val)
    {
        return glm::vec4(_val.r, _val.g, _val.b, _val.a);
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

	std::shared_ptr<Material> LoadMaterial(aiMaterial* material)
	{
		std::shared_ptr<Material> tmp = std::make_shared<Material>();

    	material->Get(AI_MATKEY_NAME, tmp->m_Name);

		auto& tmpProp = tmp->m_Properties;

		// Read material properties
		aiColor4D diffuseColor;
		aiColor4D specularColor;
		aiColor4D ambientColor;
		aiColor4D emissiveColor;
		aiColor4D transportColor;
		aiColor4D refectiveColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
		material->Get(AI_MATKEY_COLOR_TRANSPARENT, transportColor);
		material->Get(AI_MATKEY_COLOR_REFLECTIVE, refectiveColor);
		tmpProp.diffuseColor	= toVec4(diffuseColor);
		tmpProp.specularColor	= toVec4(specularColor);
		tmpProp.ambientColor	= toVec4(ambientColor);
		tmpProp.emissiveColor	= toVec4(emissiveColor);
		tmpProp.transportColor	= toVec4(transportColor);
		tmpProp.refectiveColor	= toVec4(refectiveColor);

		material->Get(AI_MATKEY_OPACITY, tmpProp.opacity);
		material->Get(AI_MATKEY_TRANSPARENCYFACTOR, tmpProp.transparentFactor);
		material->Get(AI_MATKEY_BUMPSCALING, tmpProp.bumpScaling);
		material->Get(AI_MATKEY_SHININESS, tmpProp.shininess);
		material->Get(AI_MATKEY_REFLECTIVITY, tmpProp.refelectivity);
		material->Get(AI_MATKEY_SHININESS_STRENGTH, tmpProp.shininessStrength);
		material->Get(AI_MATKEY_REFRACTI, tmpProp.refracti);

		// Retrieve the PBR properties

		/* Metallic/Roughness Workflow */
		material->Get(AI_MATKEY_BASE_COLOR, tmpProp.metallicProperties.baseColorFactor);
		material->Get(AI_MATKEY_METALLIC_FACTOR, tmpProp.metallicProperties.metallicFactor);
		material->Get(AI_MATKEY_ROUGHNESS_FACTOR, tmpProp.metallicProperties.roughnessFactor);
		material->Get(AI_MATKEY_ANISOTROPY_FACTOR, tmpProp.metallicProperties.anisotropyFactor);

		/* Specular/Glossiness Workflow */
		material->Get(AI_MATKEY_SPECULAR_FACTOR, tmpProp.glossinessProperties.specularFactor);
		material->Get(AI_MATKEY_GLOSSINESS_FACTOR, tmpProp.glossinessProperties.glossinessFactor);

		/* Sheen */
		material->Get(AI_MATKEY_SHEEN_COLOR_FACTOR, tmpProp.sheenProperties.sheenFactor);
		material->Get(AI_MATKEY_SHEEN_ROUGHNESS_FACTOR, tmpProp.sheenProperties.sheenRoughnessFactor);

		/* Clearcoat */
		material->Get(AI_MATKEY_CLEARCOAT_FACTOR, tmpProp.clearCoatProperties.clearcoatFactor);
		material->Get(AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, tmpProp.clearCoatProperties.clearcoatRoughnessFactor);

		/* Transmission */
		material->Get(AI_MATKEY_TRANSMISSION_FACTOR, tmpProp.transmissionFactor);

		/* Volume */
		material->Get(AI_MATKEY_VOLUME_THICKNESS_FACTOR, tmpProp.volumeProperties.volumeThicknessFactor);
		material->Get(AI_MATKEY_VOLUME_ATTENUATION_DISTANCE, tmpProp.volumeProperties.volumeAttenuationDistance);

		/* Emissive */
		material->Get(AI_MATKEY_EMISSIVE_INTENSITY, tmpProp.emissiveIntensity);

		// Iterate through the texture slots of the material
		for (unsigned int j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiTextureType textureType = static_cast<aiTextureType>(j);
			unsigned int textureCount = material->GetTextureCount(textureType);

			// Iterate through the textures of the current slot
			for (unsigned int k = 0; k < textureCount; k++)
			{
				aiString texturePath;
				material->GetTexture(textureType, k, &texturePath);
				std::string str = texturePath.C_Str();
				std::replace(begin(str), end(str), '\\', '/');
				if (j == 1 || j == 12 || j == 16)
					tmp->TextureMap.emplace(std::make_pair(TextureType::Albedo, str));
				else if (j == 6)
					tmp->TextureMap.emplace(std::make_pair(TextureType::Normal, str));
				else if (j == 16)
					tmp->TextureMap.emplace(std::make_pair(TextureType::Roughness, str));
				else if (j == 17)
					tmp->TextureMap.emplace(std::make_pair(TextureType::AmbientOcclusion, str));
				else if (j == 15)
					tmp->TextureMap.emplace(std::make_pair(TextureType::Metalness, str));
				else
					tmp->TextureMap.emplace(std::make_pair(TextureType::Unknown, str));
			}
		}

		return tmp;
	}


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

//			// TODO Open Animation Branch to DO
//			 //Weights
//			 std::vector<Weight> weights;
//			 for (unsigned int j = 0; j < bone->mNumWeights; j++)
//			 	weights.emplace_back(Weight{ bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight });
//
//			 m_BoneInfos.emplace_back(
//			 	std::make_shared<BoneInfo>(bone->mName.C_Str(), bone->mNumWeights, std::move(weights), toMat4(bone->mOffsetMatrix))
//			 );
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
		tmp->SetMaterial(LoadMaterial(scene->mMaterials[mesh->mMaterialIndex]));

		return tmp;
	}

	[[nodiscard]] std::vector<std::shared_ptr<Mesh>> ProcessNodes(aiNode* node, const aiScene* scene)
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

	[[nodiscard]] std::vector<std::shared_ptr<BoneInfo>> ProcessBoneNode(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<std::shared_ptr<BoneInfo>> tmp;

		// Bones
		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			const aiBone* bone = mesh->mBones[i];

			// Weights
			std::vector<Weight> weights;
			for (unsigned int j = 0; j < bone->mNumWeights; j++)
				weights.emplace_back(Weight{ bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight });

			tmp.emplace_back(
				std::make_shared<BoneInfo>(bone->mName.C_Str(), bone->mNumWeights, std::move(weights), toMat4(bone->mOffsetMatrix))
			);
		}

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
			tmp[i]->parentID = parentBoneIndex;
		}

		// Iterate through each bone and get its children indices
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			const auto& parentID = tmp[i]->parentID;
			if (parentID == -1) continue;
			tmp[parentID]->childrenIDs.emplace_back(i);
		}

		return tmp;
	}

	[[nodiscard]] std::vector<std::shared_ptr<BoneInfo>> ProcessBoneNodes(aiNode* node, const aiScene* scene)
	{
		// TODO Fix Here
		aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
		return ProcessBoneNode(mesh, scene);
	}

	[[nodiscard]] std::shared_ptr<Model> ResourceManager::LoadModel(const std::filesystem::path& _filePath)
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

		model->SetFilePath(_filePath);

		for(auto& mesh: model->GetMeshes())
		{
			for(auto&[k, v] : mesh->GetMaterial()->TextureMap)
				v = FileSystem::JoinFileRoute(_filePath.parent_path(), v);
		}

		return model;


		//if (scene->HasAnimations()) ProcessAnimationClips(scene->mRootNode, scene);
	}

	void ResourceManager::LoadTextureScope(bool flip, const std::filesystem::path& filePath, const textureLoadCallback &callback)
	{
		stbi_set_flip_vertically_on_load(flip);
		int width, height, bpp;
		unsigned char *data = stbi_load(filePath.string().c_str(), &width, &height, &bpp, 0);
		callback(width, height, bpp, data);
		if (data) {
			stbi_image_free(data);
		}
	}

	[[nodiscard]] std::shared_ptr<AnimationClip> ProcessAnimationNode(aiAnimation* animation, const aiScene* scene)
	{
		std::shared_ptr<AnimationClip> tmp = std::make_shared<AnimationClip>();
		tmp->name = animation->mName.C_Str();
		tmp->duration = animation->mDuration;
		tmp->FPS = animation->mTicksPerSecond;
		tmp->frameCount = animation->mChannels[0]->mNumPositionKeys;
		tmp->nodeAnimations.resize(animation->mNumChannels);

		// Iterate over all channels in the animation
		for (unsigned int j = 0; j < animation->mNumChannels; j++)
		{
			const aiNodeAnim* channel = animation->mChannels[j];

			tmp->nodeAnimations[j].nodeName = channel->mNodeName.C_Str();

			// Iterate over all position keyframes in the channel
			for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
			{
				const auto time = channel->mPositionKeys[k].mTime;
				const auto pos = SPW::toVec3(channel->mPositionKeys[k].mValue);
				const auto rot = SPW::toQuat(channel->mRotationKeys[k].mValue);
				const auto scaling = SPW::toVec3(channel->mScalingKeys[k].mValue);
				tmp->nodeAnimations[j].keyFrames.emplace_back(KeyFrame{ time, pos, rot, scaling });
			}
			if (channel->mNumPositionKeys > tmp->frameCount) tmp->frameCount = channel->mNumPositionKeys;
		}
		return tmp;
	}

	[[nodiscard]] std::vector<std::shared_ptr<AnimationClip>> ProcessAnimationClips(aiNode* node, const aiScene* scene)
	{
		std::vector<std::shared_ptr<AnimationClip>> animClips;
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* animation = scene->mAnimations[i];
			animClips.emplace_back(ProcessAnimationNode(animation, scene));
		}

		return animClips;
	}

	[[nodiscard]] std::shared_ptr<Skeleton> ResourceManager::LoadAnimation(const std::filesystem::path& filePath)
	{
		std::shared_ptr<Skeleton> tmp_skeleton = std::make_shared<Skeleton>();

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath.string().c_str()
			, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
			| aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return nullptr;
		}

		if (scene->HasAnimations())
		{
			tmp_skeleton->m_animClips = ProcessAnimationClips(scene->mRootNode, scene);
            return tmp_skeleton;
		}
		else
		{
			std::cout << "No AnimationClips Exist!\n";
			return nullptr;
		}
        return nullptr;
	}
}
