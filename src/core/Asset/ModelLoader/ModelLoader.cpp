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

	uint32_t findBoneId (const std::vector<BoneInfo>& m_Bones, std::string name)
	{
		for(auto bone : m_Bones)
		{
			if(bone.name == name)
				return bone.boneID;
		}
		return 0;
	}

	void ProcessAnimationNode(AssetData* asset, aiAnimation* animation, const aiScene* scene)
	{
		AnimationClip animation_clip;
		animation_clip.name			= animation->mName.C_Str();
		animation_clip.duration		= animation->mDuration;
		animation_clip.FPS			= animation->mTicksPerSecond;
		animation_clip.frameCount	= animation->mChannels[0]->mNumPositionKeys;
		animation_clip.nodeAnimations.resize(animation->mNumChannels);

		// Iterate over all channels in the animation
		for (unsigned int j = 0; j < animation->mNumChannels; j++)
		{
			const aiNodeAnim* channel = animation->mChannels[j];

			//Find boneId
			const auto& bones = asset->skeleton.m_Bones;
			uint32_t boneId = findBoneId(bones,animation->mChannels[j]->mNodeName.C_Str());

			animation_clip.nodeAnimations[j].nodeName = channel->mNodeName.C_Str();
			animation_clip.nodeAnimations[j].boneID = boneId;

			for (int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
			{
				aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
				float timeStamp = channel->mPositionKeys[positionIndex].mTime;
				KeyPosition data{};
				data.position = glm::toVec3(aiPosition);
				data.time = timeStamp;
				animation_clip.nodeAnimations[j].m_Position.push_back(data);
			}

			for (int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
			{
				aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
				float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
				KeyRotation data;
				data.rotation = glm::toQuat(aiOrientation);
				data.time = timeStamp;
				animation_clip.nodeAnimations[j].m_Rotation.push_back(data);
			}

			for (int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
			{
				aiVector3D scale = channel->mScalingKeys[scaleIndex].mValue;
				float timeStamp = channel->mScalingKeys[scaleIndex].mTime;
				KeyScale data;
				data.scaling = glm::toVec3(scale);
				data.time = timeStamp;
				animation_clip.nodeAnimations[j].m_Scaling.push_back(data);
			}

			if (channel->mNumPositionKeys > animation_clip.frameCount) animation_clip.frameCount = channel->mNumPositionKeys;
		}

		asset->skeleton.animClips.emplace_back(std::move(animation_clip));
	}

	void ProcessAnimationClips(AssetData* asset, const aiScene* scene)
	{
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* animation = scene->mAnimations[i];
			ProcessAnimationNode(asset, animation, scene);
		}
	}

	void ProcessBoneNode(AssetData* asset, aiMesh* mesh, unsigned int offset, const aiScene* scene)
	{
		// Bones
		for (unsigned int i = 0; i < mesh->mNumBones; i++)
		{
			const aiBone* bone = mesh->mBones[i];
			// Weights
			std::vector<Weight> weights;

			for (unsigned int j = 0; j < bone->mNumWeights; j++)
				weights.emplace_back(Weight{ bone->mWeights[j].mVertexId + offset, bone->mWeights[j].mWeight });

			asset->skeleton.m_Bones.emplace_back(i, bone->mName.C_Str(), bone->mNumWeights, std::move(weights), glm::toMat4(bone->mOffsetMatrix ));
		}

		// Iterate through each bone and get its parent bone index
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			const aiBone* bone = mesh->mBones[i];
			int parentBoneIndex = -1;

			// Check if the bone has a parent node
			if (bone->mNode->mParent)
			{
				// Find the parent bone index by iterating through the bone array
				for (int j = 0; j < mesh->mNumBones; j++)
				{
					if (i == j) continue;

					const aiBone* otherBone = mesh->mBones[j];
					if (otherBone->mNode == bone->mNode->mParent) {
						parentBoneIndex = j;
						break;
					}
				}
			}
			asset->skeleton.m_Bones[i].parentID = parentBoneIndex;
			asset->skeleton.m_Bones[i].boneID = i;
		}

		// Iterate through each bone and get its children indices
		for (int i = 0; i < mesh->mNumBones; i++)
		{
			const auto& parentID = asset->skeleton.m_Bones[i].parentID;
			if (parentID == -1) continue;
			asset->skeleton.m_Bones[parentID].childrenIDs.emplace_back(i);
		}
	}

	void getAllMeshNodes(std::vector<aiMesh*> &meshes, aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(mesh);
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			getAllMeshNodes(meshes, node->mChildren[i], scene);
		}
	}

	HierarchyNode ProcessHierarchy(const aiNode* src)
	{
		HierarchyNode ret;
		ret.name = src->mName.data;
		ret.transformation = glm::toMat4(src->mTransformation);

		for (size_t child_node = 0; child_node < src->mNumChildren; child_node++)
		{
			ret.children.emplace_back(ProcessHierarchy(src->mChildren[child_node]));
		}
		return ret;
	}

	std::unordered_map<std::string, SPW::BoneInfo> NameMappingBones(std::vector<BoneInfo> m_Bones)
	{
		std::unordered_map<std::string, SPW::BoneInfo> bone_map;

		int count = 0;
		for (const auto& bone : m_Bones)
		{
			std::string name = bone.name;

			if (bone_map.contains(name))
			{
				count++;
			}
			else
			{
				bone_map[name] = bone;
			}
		}
		return bone_map;
	}

	void RecursiveCalculateTransforms(AssetData* asset, SPW::AnimationClip& anim_clip, const SPW::HierarchyNode& node, const glm::mat4& parent_transform, int frame)
	{
		std::string node_name = node.name;                // get name from assimp Node
		glm::mat4   local_transform = node.transformation;		// get localTransform from assimpNode

		// auto& curr_frame_matrix = skeleton->boneMatrices[frame];
		if (auto* anim_node = FindAnimationNode(anim_clip, node_name))
		{
			float time = anim_clip.boneBindings[frame].timeStamp;
			local_transform = SPW::CalculateInterpolatedMatrix(anim_node, time);
		}
		local_transform = parent_transform * local_transform;

		if (asset->skeleton.boneMap.contains(node_name))
		{
			const auto& boneInfo = asset->skeleton.boneMap[node_name];
			const auto& boneId = boneInfo.boneID;
			const auto& boneOffset = boneInfo.offsetMatrix;
			anim_clip.boneBindings[frame].transformMatrix[boneId] = local_transform * boneOffset;
		}

		for (const auto& child : node.children)
			RecursiveCalculateTransforms(asset, anim_clip, child, local_transform, frame);
	}

	void CalculateBoneTransforms(AssetData* asset, SPW::AnimationClip& anim_clip, int frame)
	{
		RecursiveCalculateTransforms(asset, anim_clip, asset->skeleton.root, glm::mat4(1.0f), frame);
	}



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

	void ProcessMeshNode(AssetData* modelData, aiMesh* mesh, const aiScene* scene, uint32_t offset)
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

		tmpMesh.offset = offset;
		offset += tmpMesh.vertices.size();
		//		tmpMesh.vertices.size();

		if (!(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
		{
			// tmpMesh.materialID = tmpMesh.ID;
			LoadMaterial(modelData, scene->mMaterials[mesh->mMaterialIndex], tmpMesh.materialID);
		}

		modelData->meshes.emplace_back(std::move(tmpMesh));
	}

	void ProcessNodes(AssetData* modelData, aiNode* node, const aiScene* scene, uint32_t offset)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMeshNode(modelData, mesh, scene, offset);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNodes(modelData, node->mChildren[i], scene, offset);
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

		uint32_t offset = 0;
		ProcessNodes(ret.get(), scene->mRootNode, scene, offset);

// ------- LOAD ANIMATION --------------------------------
		const bool hasAnimations = scene->HasAnimations();

		// Fill the meta information
		ret->assetName	= FileSystem::GetCleanFilename(filepath.string());
		ret->path		= filepath.string();
		ret->meshURI	= FileSystem::GenerateRandomUUID();
		ret->assetID	= FileSystem::GenerateRandomUUID();

		if (hasAnimations)
		{
			// ----- ANIMATIONCLIP ----
			ProcessAnimationClips(ret.get(), scene);
			// ----- ANIMATIONCLIP ----

			// ----- BONES ----
			// Get all meshes from assimp loading
			std::vector<aiMesh*> all_meshes;
			getAllMeshNodes(all_meshes, scene->mRootNode, scene);
			// TODO: optimize this later
			unsigned int offset = 0;
			for (const auto& mesh : all_meshes)
			{
				// pass offset into
				ProcessBoneNode(ret.get(), mesh, offset, scene);
				offset += mesh->mNumVertices;
			}

			auto& ret_skeleton = ret->skeleton;
			ret_skeleton.root    = ProcessHierarchy(scene->mRootNode);
			ret_skeleton.boneMap = NameMappingBones(ret->skeleton.m_Bones);
			// ----- BONES ----

			// ---------- Calculate Bone Transforms ----------
			for(int clip = 0 ; clip < ret_skeleton.animClips.size(); ++clip)
			{
				ret_skeleton.animClips[clip].finalBoneMatrices.resize(ret_skeleton.boneMap.size(), glm::mat4(1.0f));

				int frameCount = 60;
				ret_skeleton.animClips[clip].boneBindings.resize(frameCount);
				for (int i = 0; i < frameCount; ++i)
				{
					ret_skeleton.animClips[clip].boneBindings[i].timeStamp = ret_skeleton.animClips[clip].duration / static_cast<float>(frameCount - 1) * static_cast<float>(i);
					ret_skeleton.animClips[clip].boneBindings[i].transformMatrix.resize(ret_skeleton.boneMap.size(), glm::mat4(1.0f));
				}

				for (int i = 0; i < frameCount; ++i)
				{
					CalculateBoneTransforms(ret.get(), ret_skeleton.animClips[clip], i);
				}
			}
		}
		else
			std::cout << "No AnimationClips Exist! Only Return Static Data! \n";


		return std::move(ret);
	}
}
