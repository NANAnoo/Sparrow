/*
* @date   2023-03-31
* @author dudu
* @brief  ${FILE_DESCRIPTION}
 */
#pragma once

#include "Asset/Asset.hpp"
#include "Asset/AssetData/MaterialData.h"
#include "Asset/AssetData/MeshData.h"
#include "Asset/AssetData/ModelData.h"
#include "Model/Mesh.h"
namespace SPW
{

  struct ModelData
  {
    // ModelData model;
    // AssetType type;
    std::string assetID;
    std::string name;
    std::string meshURI;
    std::string path; // Not ser this, but use to pass parameters

  	std::vector<Mesh> meshes;
    std::vector<MaterialData> materials;
    std::unordered_map<std::string, std::string> textures;
  };

  class ModelLoader
  {
  public:
    static std::unique_ptr<ModelData> LoadModel(const std::string& filename);
  };
}