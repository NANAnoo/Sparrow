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

namespace SPW
{

  struct ModelDataRet
  {
    ModelData model;
    std::vector<MeshData> meshes;
    std::vector<MaterialData> materials;
    std::unordered_map<std::string, std::string> textures;
  };

  class ModelLoader
  {
  public:
    static std::unique_ptr<ModelDataRet> LoadModel(const std::string& filename);
  };
}