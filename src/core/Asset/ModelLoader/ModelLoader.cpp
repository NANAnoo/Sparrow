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

	void ProcessAnimationNode(AssetData* asset, aiAnimation* animation, const aiScene* scene)
	{
		AnimationClip animation_clip;
		animation_clip.name			= animation->mName.C_Str();
		animation_clip.duration		= animation->mDuration;
		animation_clip.FPS			= animation->mTicksPerSecond;
		animation_clip.frameCount	= animation->mChannels[0]->mNumPositionKeys;
		animation_clip.animNodes.resize(animation->mNumChannels);

		// Iterate over all channels in the animation
		for (unsigned int j = 0; j < animation->mNumChannels; j++)
		{
			const aiNodeAnim* channel = animation->mChannels[j];

			animation_clip.animNodes[j].name = channel->mNodeName.C_Str();

			for (int positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
			{
				aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
				float timeStamp = channel->mPositionKeys[positionIndex].mTime;
				KeyPosition data{};
				data.position = glm::toVec3(aiPosition);
				data.time = timeStamp;
				animation_clip.animNodes[j].positionKeys.push_back(data);
			}

			for (int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
			{
				aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
				float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
				KeyRotation data;
				data.rotation = glm::toQuat(aiOrientation);
				data.time = timeStamp;
				animation_clip.animNodes[j].rotationKeys.push_back(data);
			}

			for (int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
			{
				aiVector3D scale = channel->mScalingKeys[scaleIndex].mValue;
				float timeStamp = channel->mScalingKeys[scaleIndex].mTime;
				KeyScale data;
				data.scaling = glm::toVec3(scale);
				data.time = timeStamp;
				animation_clip.animNodes[j].scalingKeys.push_back(data);
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

	// void ProcessBoneNode(AssetData* asset, aiMesh* mesh, unsigned int offset, const aiScene* scene, std::vector<BoneInfo>& t_BonesInLoading)
	// {
	// 	// Bones
	// 	for (size_t i = 0; i < mesh->mNumBones; i++)
	// 	{
	// 		// Weights
	// 		std::vector<Weight> weights;
	// 		for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
	// 		{
	// 			weights.emplace_back(Weight{ mesh->mBones[i]->mWeights[j].mVertexId + offset, mesh->mBones[i]->mWeights[j].mWeight });
	// 		}
	// 		BoneInfo boneInfo;
	// 		boneInfo.boneID = i;
	// 		boneInfo.name = mesh->mBones[i]->mName.C_Str();
	// 		boneInfo.weights = std::move(weights);
	// 		boneInfo.offsetMatrix = glm::toMat4(mesh->mBones[i]->mOffsetMatrix);
	// 		t_BonesInLoading.emplace_back(std::move(boneInfo));
	// 	}
	// }

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

	std::unordered_map<std::string, BoneInfo> GetUniqueBoneMap(const std::vector<BoneInfo>& repeatedBones)
	{
		std::unordered_map<std::string, BoneInfo> bone_map;

		for (const auto& bone : repeatedBones)
		{
			std::string name = bone.name;

			if (!bone_map.contains(name))
			{
				bone_map[name] = bone;
			}
		}
		return bone_map;
	}

	void RecursiveCalculateTransforms(AssetData* asset, AnimationClip& anim_clip, const SPW::HierarchyNode& node, const glm::mat4& parent_transform, int frame)
	{
		std::string node_name = node.name;                // get name from assimp Node
		glm::mat4   local_transform = node.transformation;		// get localTransform from assimpNode

		// auto& curr_frame_matrix = skeleton->boneMatrices[frame];
		if (auto* anim_node = FindAnimationNode(anim_clip, node_name))
		{
			float time = anim_clip.matrixPerFrames[frame].timeStamp;
			local_transform = CalculateInterpolatedMatrix(anim_node, time);
		}
		local_transform = parent_transform * local_transform;

		if (asset->skeleton.boneMap.contains(node_name))
		{
			const auto& boneInfo = asset->skeleton.boneMap[node_name];
			const auto& boneId = boneInfo.boneID;
			const auto& boneOffset = boneInfo.offsetMatrix;
			anim_clip.matrixPerFrames[frame].transformMatrix[boneId] = local_transform * boneOffset;
		}

		for (const auto& child : node.children)
			RecursiveCalculateTransforms(asset, anim_clip, child, local_transform, frame);
	}

	void CalculateBoneTransforms(AssetData* asset, AnimationClip& anim_clip, int frame)
	{
		RecursiveCalculateTransforms(asset, anim_clip, asset->skeleton.hierarchy, glm::mat4(1.0f), frame);
	}

	void LoadMaterial(AssetData* modelData, aiMaterial* material, const std::string& id_str)
	{
		MaterialData tmpMaterial{};

		tmpMaterial.ID = id_str;

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
			if (texturePath.length == 0)
				material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

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

	Mesh ProcessMeshNode(AssetData* modelData, aiMesh* mesh, const aiScene* scene)
	{
		Mesh tmpMesh{};
		tmpMesh.materialID = FileSystem::GenerateRandomUUID();

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

		return (std::move(tmpMesh));
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

		// Get all meshes from assimp loading
		std::vector<aiMesh*> all_meshes;
		getAllMeshNodes(all_meshes, scene->mRootNode, scene);

		size_t offset = 0;
		for (const auto& mesh : all_meshes)
		{
			ret->meshes.emplace_back(ProcessMeshNode(ret.get(), mesh, scene));
			ret->meshes.back().offset = offset;

			offset += mesh->mNumVertices;
		}

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

			// ---------- BONES ----------
			// Get all meshes from assimp loading
			unsigned int anim_offset = 0;

			struct Weight
			{
				unsigned int vertexID;
				float value;
			};

			struct BoneInLoading
			{
				int32_t boneID = -1;
				std::string name;
				std::vector<Weight> weights;
				glm::mat4 offsetMatrix;
			};

			std::vector<BoneInLoading> t_BonesInLoading;
			for (const auto& mesh : all_meshes)
			{
				// ProcessBoneNode(ret.get(), mesh, anim_offset, scene, t_BonesInLoading);
				{
					// Bones
					for (size_t i = 0; i < mesh->mNumBones; i++)
					{
						// Weights
						std::vector<Weight> weights;
						for (size_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
						{
							weights.emplace_back(Weight{ (mesh->mBones[i]->mWeights[j].mVertexId + anim_offset), mesh->mBones[i]->mWeights[j].mWeight });
						}
						BoneInLoading boneInfo;
						boneInfo.boneID = i;
						boneInfo.name = mesh->mBones[i]->mName.C_Str();
						boneInfo.weights = std::move(weights);
						boneInfo.offsetMatrix = glm::toMat4(mesh->mBones[i]->mOffsetMatrix);
						t_BonesInLoading.emplace_back(std::move(boneInfo));
					}
				}
				anim_offset += mesh->mNumVertices;
			}

			auto& ret_skeleton = ret->skeleton;
			ret_skeleton.hierarchy    = ProcessHierarchy(scene->mRootNode);
			// ret_skeleton.boneMap = GetUniqueBoneMap(t_BonesInLoading);
			{
				for (const auto& bone : t_BonesInLoading)
				{
					std::string name = bone.name;

					if (!ret_skeleton.boneMap.contains(name))
					{
						BoneInfo boneInfo;
						boneInfo.boneID = bone.boneID;
						boneInfo.name = bone.name;
						// boneInfo.weights = bone.weights;
						boneInfo.offsetMatrix = bone.offsetMatrix;

						ret_skeleton.boneMap[name] = boneInfo;
					}
				}
			}

			// ExtractWeightFromBones(ret.get(), t_BonesInLoading);
			{
				struct VerMapBone
				{
					//Vertex -> <BoneID> -> (stores Index of finalBoneMatrix)
					std::vector<uint32_t> boneID;
					//Weight -> <double) -> (stores corresponding weight; (boneID[0] - Weight[0]) )
					std::vector<float> weight;
				};

				// VertBoneMap vertBoneMap;

				ret->skeleton.vertexWeightMap.boneCount = t_BonesInLoading.size();

				unsigned int numVertex = 0;
				std::vector<VerMapBone> verMapBone;
				for (const auto& m : ret->meshes)
				{
					numVertex += m.vertices.size();
				}

				verMapBone.resize(numVertex);
				for (auto boneInfo : t_BonesInLoading)
				{
					for (Weight weight : boneInfo.weights)
					{
						uint32_t vertexID = weight.vertexID;
						uint32_t boneID = boneInfo.boneID;
						float value = weight.value;

						verMapBone[vertexID].boneID.push_back(boneID);
						verMapBone[vertexID].weight.push_back(value);
					}
				}

				//Get number of vertices
				ret->skeleton.vertexWeightMap.startIndex.reserve(verMapBone.size());

				//For every vertex
				int startInd = 0;
				for (auto temp : verMapBone)
				{
					ret->skeleton.vertexWeightMap.startIndex.push_back(startInd);

					for (int j = 0; j < temp.boneID.size(); j++)
					{
						ret->skeleton.vertexWeightMap.boneID.push_back(temp.boneID[j]);
						ret->skeleton.vertexWeightMap.weights.push_back(temp.weight[j]);
					}
					startInd += temp.boneID.size();
					ret->skeleton.vertexWeightMap.count.push_back(temp.boneID.size());
				}

				// assetdata->skeleton.vertexWeightMap = vertBoneMap;
			}
			// ---------- BONES ----------

			// ---------- Calculate Bone Transforms ----------
			for(int clip = 0 ; clip < ret_skeleton.animClips.size(); ++clip)
			{
				ret_skeleton.animClips[clip].boneMatrices.resize(ret_skeleton.boneMap.size(), glm::mat4(1.0f));

				int frameCount = 60;
				ret_skeleton.animClips[clip].matrixPerFrames.resize(frameCount);
				for (int i = 0; i < frameCount; ++i)
				{
					ret_skeleton.animClips[clip].matrixPerFrames[i].timeStamp = ret_skeleton.animClips[clip].duration / static_cast<float>(frameCount - 1) * static_cast<float>(i);
					ret_skeleton.animClips[clip].matrixPerFrames[i].transformMatrix.resize(ret_skeleton.boneMap.size(), glm::mat4(1.0f));
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

