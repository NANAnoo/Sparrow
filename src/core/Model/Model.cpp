#include "Model/Model.h"

#include "assimp/code/AssetLib/3MF/3MFXmlTags.h"

namespace SPW
{
    Model::Model(const std::filesystem::path& _FilePath, bool gamma)
        :m_FilePath(_FilePath), m_Directory(_FilePath.parent_path())
    {
        m_Meshes.resize(0);
    }

    // void Model::LoadModel(const std::filesystem::path& _filePath)
    // {
    //     const auto& fileExtension = _filePath.extension();
    //     LoadAssimp(_filePath);
    // }
}
