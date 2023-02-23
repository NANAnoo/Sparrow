#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"

namespace SPW
{
  namespace fs = std::filesystem;

  inline glm::vec3 asVec3(const aiVector3D& _val)
  {
    return glm::vec3(_val.x, _val.y, _val.z);
  }

  inline glm::quat asQuat(const aiQuaternion& _val)
  {
    return glm::quat(glm::vec4(_val.x, _val.y, _val.z, _val.w));
  }

  inline glm::vec3 quat_2_euler (const glm::quat& _quat)
  {
    return glm::eulerAngles(_quat);
  }

  // `mat` is an `aiMatrix4x4`
  glm::mat4 to_glm_mat4(const aiMatrix4x4& mat) {
    glm::mat4 result;
    result[0][0] = mat.a1; result[1][0] = mat.a2; result[2][0] = mat.a3; result[3][0] = mat.a4;
    result[0][1] = mat.b1; result[1][1] = mat.b2; result[2][1] = mat.b3; result[3][1] = mat.b4;
    result[0][2] = mat.c1; result[1][2] = mat.c2; result[2][2] = mat.c3; result[3][2] = mat.c4;
    result[0][3] = mat.d1; result[1][3] = mat.d2; result[2][3] = mat.d3; result[3][3] = mat.d4;
    return result;
  }

  std::shared_ptr<Mesh> Model::ProcessMeshNode(aiMesh* mesh, const aiScene* scene)
  {
    std::shared_ptr<Mesh> tmp = std::make_shared<Mesh>();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
//    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
      // positions
      glm::vec3 tmpPosition = glm::vec3(
          mesh->mVertices[i].x,
          mesh->mVertices[i].y,
          mesh->mVertices[i].z);

      // normals
      glm::vec3 tmpNormal = mesh->HasNormals() ?
                                               glm::vec3(
                                                   mesh->mNormals[i].x,
                                                   mesh->mNormals[i].y,
                                                   mesh->mNormals[i].z) :
                                               glm::vec3();

      // texture coordinates
      bool uv_exist = mesh->mTextureCoords[0];
      glm::vec2 tmpUV = uv_exist?
                                 glm::vec2(
                                     mesh->mTextureCoords[0][i].x,
                                     mesh->mTextureCoords[0][i].y) :
                                 glm::vec2();

      // tangents
      glm::vec3 tmpTangent = uv_exist ?
                             glm::vec3(
                                 mesh->mTangents[i].x,
                                 mesh->mTangents[i].y,
                                 mesh->mTangents[i].z) :
                             glm::vec3();

      // Bitangents
      glm::vec3 tmpBitangent = uv_exist ?
                             glm::vec3(
                                 mesh->mBitangents[i].x,
                                 mesh->mBitangents[i].y,
                                 mesh->mBitangents[i].z) :
                             glm::vec3();

      tmp->vertices.emplace_back(Vertex{tmpPosition, tmpNormal, tmpUV, tmpTangent, tmpBitangent});
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
      aiFace face = mesh->mFaces[i];
      // retrieve all indices of the face and store them in the indices vector
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        // indices.push_back(face.mIndices[j]);
        tmp->indices.emplace_back(face.mIndices[j]);
    }

    // Iterate over the bones in the mesh
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
      const aiBone* bone = mesh->mBones[i];

      // Get the offset matrix for the bone

      std::vector<Weight> tmp_Weights;
      // Iterate over the weights for the bone
      for (unsigned int j = 0; j < bone->mNumWeights; j++) {

        tmp_Weights.emplace_back(Weight{ bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight });
        // Do something with the vertex weight and the bone offset matrix
      }


      m_BoneInfos.emplace_back(
          std::make_shared<BoneInfo>(
              bone->mName.C_Str(),
              bone->mNumWeights,
              std::move(tmp_Weights),
              to_glm_mat4(bone->mOffsetMatrix)));
    }

    int numBones = mesh->mNumBones;

    // Create a map of bone name to index for efficient lookups
    std::map<std::string, int> boneIndexMap;
    for (int i = 0; i < numBones; i++) {
      aiBone* bone = mesh->mBones[i];
      boneIndexMap[bone->mName.C_Str()] = i;
    }

    // Iterate through each bone and get its parent bone index
    for (int i = 0; i < numBones; i++) {
      aiBone* bone = mesh->mBones[i];
      int parentBoneIndex = -1;

      // Check if the bone has a parent node
      if (bone->mNode->mParent) {
        // Find the parent bone index by iterating through the bone array
        for (int j = 0; j < numBones; j++) {
          if (i == j) {
            continue;
          }

          aiBone* otherBone = mesh->mBones[j];
          if (otherBone->mNode == bone->mNode->mParent) {
            parentBoneIndex = j;
            break;
          }
        }
      }

      std::cout << "Bone " << i << mesh->mBones[i]->mName.C_Str()  << " parent: " << parentBoneIndex << std::endl;
    }

    // Deal with Materials
//    MaterialProperties defaultMaterial;
//    std::shared_ptr<Material> tmpMaterial = std::make_shared<Material>(defaultMaterial);
//    tmp->m_Material = std::move(tmpMaterial);

    return tmp;
  }

  void Model::ProcessNodes(aiNode* node, const aiScene* scene)
  {
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      m_Meshes.emplace_back((ProcessMeshNode(mesh, scene)));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
      ProcessNodes(node->mChildren[i], scene);
    }
  }

  std::shared_ptr<AnimationClip> Model::ProcessAnimationNode(aiAnimation* animation, const aiScene* scene)
  {
    std::shared_ptr<AnimationClip> tmp = std::make_shared<AnimationClip>();

    tmp->name = animation->mName.C_Str();
    tmp->duration = animation->mDuration;
    tmp->FPS = animation->mTicksPerSecond;
    tmp->nodeAnimations.resize(animation->mNumChannels);

    // Iterate over all channels in the animation
    for (unsigned int j = 0; j < animation->mNumChannels; j++)
    {
      const aiNodeAnim* channel = animation->mChannels[j];

      tmp->nodeAnimations[j].nodeName = channel->mNodeName.C_Str();
      tmp->nodeAnimations[j].positionKeys.resize(channel->mNumPositionKeys);
      tmp->nodeAnimations[j].rotationKeys.resize(channel->mNumRotationKeys);
      tmp->nodeAnimations[j].scalingKeys.resize(channel->mNumScalingKeys);

      // Iterate over all position keyframes in the channel
      for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
      {
        tmp->nodeAnimations[j].positionKeys[k].value = asVec3(channel->mPositionKeys[k].mValue);
        tmp->nodeAnimations[j].positionKeys[k].time = channel->mPositionKeys[k].mTime;
        // Do something with the position and time values
      }

      // Iterate over all rotation keyframes in the channel
      for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
      {
        aiQuaternion aiq = channel->mRotationKeys[k].mValue;
        glm::quat glmq = asQuat(aiq);
        glm::vec3 glmvec3 = quat_2_euler(glmq);

        tmp->nodeAnimations[j].rotationKeys[k].value = glmvec3;
        tmp->nodeAnimations[j].rotationKeys[k].time = channel->mRotationKeys[k].mTime;
        // Do something with the rotation and time values
      }

      // Iterate over all scaling keyframes in the channel
      for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
      {
        tmp->nodeAnimations[j].scalingKeys[k].value = asVec3(channel->mScalingKeys[k].mValue);
        tmp->nodeAnimations[j].scalingKeys[k].time = channel->mScalingKeys[k].mTime;
        // Do something with the scaling and time values
      }
    }
    return tmp;
  }

  void Model::ProcessAnimationClips(aiNode* node, const aiScene* scene)
  {
    // Iterate over all animtion clips
    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
      aiAnimation* animation = scene->mAnimations[i];

      m_AnimationClips.emplace_back((ProcessAnimationNode(animation, scene)));
    }
  }

  void Model::LoadAssimp(const std::filesystem::path& _filePath)
  {
    // Create an instance of the Assimp importer
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_filePath.string().c_str(), 
												 aiProcess_Triangulate |
                                                 aiProcess_GenSmoothNormals |
                                                 aiProcess_FlipUVs |
                                                 aiProcess_CalcTangentSpace |
                                                 aiProcess_PopulateArmatureData);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return;
    }
    fs::path directory = _filePath.parent_path();

    ProcessNodes(scene->mRootNode, scene);

    if (scene->HasAnimations())
      ProcessAnimationClips(scene->mRootNode, scene);
  }
}
