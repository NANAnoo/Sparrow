/*
* @date   2023-03-31
* @author dudu
* @brief  ${FILE_DESCRIPTION}
 */
#include "ModelLoader.h"
#include "IO/FileSystem.h"

#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/postprocess.h>
#include <assimp/include/assimp/scene.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

namespace glm
{
	glm::vec3 toVec3(const aiVector3D& _val)
	{
		return glm::vec3(_val.x, _val.y, _val.z);
	}

	glm::vec3 toVec3(const aiColor4D& _val)
	{
		return glm::vec3(_val.r, _val.g, _val.b);
	}

	glm::vec4 toVec4(const aiColor4D& _val)
	{
		return glm::vec4(_val.r, _val.g, _val.b, _val.a);
	}


	glm::quat toQuat(const aiQuaternion& _val)
	{
		return glm::quat(_val.w, _val.x, _val.y, _val.z);
	}

	glm::vec3 quatToEuler(const glm::quat& _quat)
	{
		return glm::eulerAngles(_quat);
	}

	// `mat` is an `aiMatrix4x4`
	glm::mat4 toMat4(const aiMatrix4x4& mat)
	{
		glm::mat4 result;
		result[0][0] = mat.a1;
		result[1][0] = mat.a2;
		result[2][0] = mat.a3;
		result[3][0] = mat.a4;
		result[0][1] = mat.b1;
		result[1][1] = mat.b2;
		result[2][1] = mat.b3;
		result[3][1] = mat.b4;
		result[0][2] = mat.c1;
		result[1][2] = mat.c2;
		result[2][2] = mat.c3;
		result[3][2] = mat.c4;
		result[0][3] = mat.d1;
		result[1][3] = mat.d2;
		result[2][3] = mat.d3;
		result[3][3] = mat.d4;
		return result;
	}
}

namespace SPW
{
	void LoadMaterial(AssetData* modelData, aiMaterial* material, const std::string& id_str)
	{
		MaterialData tmpMaterial{};

		// tmpMaterial->ID = 0; TODO ID Setting
		tmpMaterial.ID = id_str;
		// tmpMaterial.type = AssetType::Material;
		// tmpMaterial.name = material->GetName().C_Str();
		// tmpMaterial.path = modelData->path;
		// tmpMaterial.

		// material->Get(AI_MATKEY_NAME, tmpMaterial.name);

		auto& tmpProp = tmpMaterial.m_Properties;

		// AlbedoConstant is a vec3
		aiColor4D baseColor;
		material->Get(AI_MATKEY_BASE_COLOR, baseColor);
		tmpProp.albedoConstant = glm::toVec3(baseColor);

		// float
		material->Get(AI_MATKEY_METALLIC_FACTOR, tmpProp.metallicConstant);
		material->Get(AI_MATKEY_ROUGHNESS_FACTOR, tmpProp.roughnessConstant);
		material->Get(AI_MATKEY_ANISOTROPY_FACTOR, tmpProp.AOConstant);

		aiColor4D diffuseColor;
		aiColor4D specularColor;
		aiColor4D ambientColor;
		aiColor4D emissiveColor;
		aiColor4D transparentColor;
		aiColor4D refectiveColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
		material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparentColor);
		material->Get(AI_MATKEY_COLOR_REFLECTIVE, refectiveColor);

		// diffuse, specular, ambient, and emissive as vec3
		tmpProp.diffuse = glm::toVec3(diffuseColor);
		tmpProp.specular = glm::toVec3(specularColor);
		tmpProp.ambient = glm::toVec3(ambientColor);
		tmpProp.emissive = glm::toVec3(emissiveColor);

		// transparent and refection as vec3
		tmpProp.transparent = glm::toVec3(transparentColor);
		tmpProp.refection = glm::toVec3(refectiveColor);

		// float
		material->Get(AI_MATKEY_EMISSIVE_INTENSITY, tmpProp.emissiveIntensity);
		material->Get(AI_MATKEY_SHININESS, tmpProp.specularPower);
		material->Get(AI_MATKEY_TRANSPARENCYFACTOR, tmpProp.transparentIntensity);
		material->Get(AI_MATKEY_REFRACTI, tmpProp.refractionIntensity);


		const auto& basepath = FileSystem::ToFsPath(modelData->path).parent_path();
		aiString texturePath;

		{
			material->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);
			std::string str = FileSystem::JoinFileRoute(basepath, texturePath.C_Str());
			if (texturePath.length != 0)
			{
				std::string uuid = FileSystem::GenerateRandomUUID();
				modelData->textures.emplace(uuid, str);
				tmpMaterial.m_TextureIDMap.emplace(std::make_pair(TextureMapType::Albedo, uuid));
			}
		}

		{
			material->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
			std::string str = FileSystem::JoinFileRoute(basepath, texturePath.C_Str());
			if (texturePath.length != 0)
			{
				std::string uuid = FileSystem::GenerateRandomUUID();
				modelData->textures.emplace(uuid, str);
				tmpMaterial.m_TextureIDMap.emplace(std::make_pair(TextureMapType::Normal, uuid));
			}
		}

		{
			material->GetTexture(aiTextureType_METALNESS, 0, &texturePath);
			std::string str = FileSystem::JoinFileRoute(basepath, texturePath.C_Str());
			if (texturePath.length != 0)
			{
				std::string uuid = FileSystem::GenerateRandomUUID();
				modelData->textures.emplace(uuid, str);
				tmpMaterial.m_TextureIDMap.emplace(std::make_pair(TextureMapType::Metalness, uuid));
			}
		}

		{
			material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texturePath);
			std::string str = FileSystem::JoinFileRoute(basepath, texturePath.C_Str());

			if (texturePath.length != 0)
			{
				std::string uuid = FileSystem::GenerateRandomUUID();
				modelData->textures.emplace(uuid, str);
				tmpMaterial.m_TextureIDMap.emplace(std::make_pair(TextureMapType::Roughness, uuid));
			}
		}

		{
			material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath);
			std::string str = FileSystem::JoinFileRoute(basepath, texturePath.C_Str());

			if (texturePath.length != 0)
			{
				std::string uuid = FileSystem::GenerateRandomUUID();
				modelData->textures.emplace(uuid, str);
				tmpMaterial.m_TextureIDMap.emplace(std::make_pair(TextureMapType::AmbientOcclusion, uuid));
			}
		}

		modelData->materials.emplace_back(std::move(tmpMaterial));
	}

	//void ProcessBoneNode(Skeleton* skeleton, aiMesh* mesh, const aiScene* scene)
	//{
	//  // Bones
	//  for (unsigned int i = 0; i < mesh->mNumBones; i++)
	//  {
	//    const aiBone* bone = mesh->mBones[i];
	//
	//    std::vector<Weight> weights;
	//    for (unsigned int j = 0; j < bone->mNumWeights; j++)
	//      weights.emplace_back(Weight{ bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight });
	//
	//    skeleton->m_Bones.emplace_back(std::make_unique<BoneInfo>(
	//        bone->mName.C_Str(),
	//        std::move(weights),
	//        glm::toMat4(bone->mOffsetMatrix)));
	//  }
	//
	//  // Iterate through each bone and get its parent bone index
	//  for (int i = 0; i < mesh->mNumBones; i++)
	//  {
	//    aiBone* bone = mesh->mBones[i];
	//    int parentBoneIndex = -1;
	//
	//    // Check if the bone has a parent node
	//    if (bone->mNode->mParent)
	//    {
	//      // Find the parent bone index by iterating through the bone array
	//      for (int j = 0; j < mesh->mNumBones; j++)
	//      {
	//        if (i == j) continue;
	//
	//        aiBone* otherBone = mesh->mBones[j];
	//        if (otherBone->mNode == bone->mNode->mParent) {
	//          parentBoneIndex = j;
	//          break;
	//        }
	//      }
	//    }
	//    skeleton->m_Bones[i]->parentID = parentBoneIndex;
	//    skeleton->m_Bones[i]->boneID = i;
	//  }
	//
	//  // Iterate through each bone and get its children indices
	//  for (int i = 0; i < mesh->mNumBones; i++)
	//  {
	//    const auto& parentID = skeleton->m_Bones[i]->parentID;
	//    if (parentID == -1) continue;
	//    skeleton->m_Bones[parentID]->childrenIDs.emplace_back(i);
	//  }
	//}

	//void ProcessBoneNodes(Skeleton* skeleton, aiNode* node, const aiScene* scene)
	//{
	//  // model->ID = 0; TODO ID Setting
	//  for (size_t i = 0; i < node->mNumMeshes; i++)
	//  {
	//    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
	//    ProcessBoneNode(skeleton, mesh, scene);
	//  }
	//
	//  for (size_t i = 0; i < node->mNumChildren; i++)
	//  {
	//    ProcessBoneNodes(skeleton, node->mChildren[i], scene);
	//  }
	//}
	//
	//void ReadHierarchyData(AssimpNodeData& rootNode, const aiNode* src)
	//{
	//  assert(src);
	//
	//  rootNode.name = src->mName.data;
	//  rootNode.transformation = glm::toMat4(src->mTransformation);
	//  rootNode.childrenCount = src->mNumChildren;
	//
	//  for (int i = 0; i < src->mNumChildren; i++)
	//  {
	//    AssimpNodeData newData;
	//    ReadHierarchyData(newData, src->mChildren[i]);
	//    rootNode.children.push_back(newData);
	//  }
	//}

	//uint32_t FindBoneId(Skeleton* skeleton, std::string name)
	//{
	//  for (const auto& bone : skeleton->m_Bones)
	//  {
	//    if (bone->name == name)
	//      return bone->boneID;
	//  }
	//}

	//std::unique_ptr<AnimationClip>
	//ProcessAnimationNode(Skeleton* skeleton, aiAnimation* animation, const aiScene* scene)
	//{
	//  std::unique_ptr<AnimationClip> tmpAnimation = std::make_unique<AnimationClip>();
	//  tmpAnimation->name = animation->mName.C_Str();
	//  tmpAnimation->duration = animation->mDuration;
	//  tmpAnimation->FPS = animation->mTicksPerSecond;
	//  tmpAnimation->frameCount = animation->mChannels[0]->mNumPositionKeys;
	//
	//  tmpAnimation->nodeAnimations.resize(animation->mNumChannels);
	//  // Iterate over all channels in the animation
	//  for (unsigned int j = 0; j < animation->mNumChannels; j++)
	//  {
	//    const aiNodeAnim* channel = animation->mChannels[j];
	//
	//    tmpAnimation->nodeAnimations[j].nodeName = channel->mNodeName.C_Str();
	//    tmpAnimation->nodeAnimations[j].boneID   = FindBoneId(skeleton, animation->mChannels[j]->mNodeName.C_Str());
	//
	//    for (int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
	//    {
	//      tmpAnimation->nodeAnimations[j].m_Position.emplace_back(
	//          KeyPosition
	//          {
	//              channel->mPositionKeys[positionIndex].mTime,
	//              glm::toVec3(channel->mPositionKeys[positionIndex].mValue)
	//          });
	//    }
	//
	//    for (int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
	//    {
	//      tmpAnimation->nodeAnimations[j].m_Rotation.emplace_back(
	//          KeyRotation
	//          {
	//              channel->mRotationKeys[rotationIndex].mTime,
	//              glm::toQuat(channel->mRotationKeys[rotationIndex].mValue)
	//          });
	//    }
	//
	//    for (int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
	//    {
	//      tmpAnimation->nodeAnimations[j].m_Scaling.emplace_back(
	//          KeyScale
	//          {
	//              channel->mScalingKeys[scaleIndex].mTime ,
	//              glm::toVec3(channel->mScalingKeys[scaleIndex].mValue)
	//          });
	//    }
	//
	//    if (channel->mNumPositionKeys > tmpAnimation->frameCount)
	//      tmpAnimation->frameCount = channel->mNumPositionKeys;
	//  }
	//
	//  return tmpAnimation;
	//}
	//
	//void ProcessAnimationClips(Skeleton* skeleton, aiNode* node, const aiScene* scene)
	//{
	//  // model->ID = 0; TODO ID Setting
	//  for (uint32_t i = 0; i < scene->mNumAnimations; i++)
	//  {
	//    aiAnimation* animation = scene->mAnimations[i];
	//    skeleton->m_animClips.emplace_back(ProcessAnimationNode(skeleton, animation, scene));
	//  }
	//}


	void ProcessMeshNode(AssetData* modelData, aiMesh* mesh, const aiScene* scene)
	{
		// modelData->model.m_MeshIDs.emplace_back(FileSystem::GenerateRandomUUID());
		Mesh tmpMesh{};
		tmpMesh.materialID = FileSystem::GenerateRandomUUID();
		// tmpMesh.type = AssetType::Mesh;
		// tmpMesh.name = mesh->mName.C_Str();
		// tmpMesh.path = modelData->model.path;

		// Vertices
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			// positions
			glm::vec3 position{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

			// normals
			glm::vec3 normal = mesh->HasNormals()
				                   ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
				                   : glm::vec3();

			// texture coordinates
			const bool uv_exist = mesh->mTextureCoords[0];
			glm::vec2 uv = uv_exist
				               ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
				               : glm::vec2();

			// tangents
			glm::vec3 tangent = uv_exist
				                    ? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)
				                    : glm::vec3();

			// bitangents
			glm::vec3 bitangent = uv_exist
				                      ? glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y,
				                                  mesh->mBitangents[i].z)
				                      : glm::vec3();

			tmpMesh.vertices.emplace_back(position, normal, uv, tangent, bitangent);
		}

		// Indices
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			const auto& face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				tmpMesh.indices.emplace_back(face.mIndices[j]);
		}

		if (!(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
		{
			// tmpMesh.materialID = tmpMesh.ID;
			LoadMaterial(modelData, scene->mMaterials[mesh->mMaterialIndex], tmpMesh.materialID);
		}

		modelData->meshes.emplace_back(std::move(tmpMesh));
	}

	void ProcessNodes(AssetData* modelData, aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMeshNode(modelData, mesh, scene);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNodes(modelData, node->mChildren[i], scene);
		}
	}

	std::unique_ptr<AssetData> ModelLoader::LoadModel(const std::string& filename)
	{
		std::unique_ptr<AssetData> ret = std::make_unique<AssetData>();

		// Create an instance of the Assimp importer
		const FilePath filepath = filename.c_str();
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename.c_str()
		                                         , aiProcess_Triangulate
		                                         | aiProcess_GenSmoothNormals
		                                         | aiProcess_FlipUVs
		                                         | aiProcess_CalcTangentSpace
		                                         | aiProcess_PopulateArmatureData);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return nullptr;
		}

		std::cout << "SUCESS::ASSIMP::" << filename << std::endl;

		const bool hasAnimations = scene->HasAnimations();

		// Fill the meta information
		// ret->model.ID   = FileSystem::GenerateRandomUUID();
		// ret->type = hasAnimations ? AssetType::AnimatedModel : AssetType::StaticModel;
		ret->assetName	= FileSystem::GetCleanFilename(filepath.string());
		ret->path		= filepath.string();
		ret->meshURI	= FileSystem::GenerateRandomUUID(); /* trace directory for storing related resources*/
		ret->assetID	= FileSystem::GenerateRandomUUID(); /* trace directory for storing related resources*/

		if (hasAnimations)
		{
			// ProcessBoneNodes(ret.get(), scene->mRootNode, scene);
			// ProcessAnimationClips(ret.get(), scene->mRootNode, scene);

			// AssimpNodeData root;
			// ReadHierarchyData(root, scene->mRootNode);
			// ret->m_assimpRootNode = root;
		}
		else
			std::cout << "No AnimationClips Exist! Only Return Static Data! \n";

		ProcessNodes(ret.get(), scene->mRootNode, scene);

		return std::move(ret);
	}
}
