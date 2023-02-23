#include "Model/Model.h"

namespace SPW
{
    Model::Model(const std::filesystem::path& _FilePath, bool gamma)
        :m_FilePath(_FilePath), m_Directory(_FilePath.parent_path())
    {
        m_Meshes.resize(0);
        LoadModel(_FilePath);
    }

    Model::Model(const std::shared_ptr<Mesh>& mesh)
    {
        m_Meshes.emplace_back(mesh);
    }

    void Model::LoadModel(const std::filesystem::path& _filePath)
    {
        const auto& fileExtension = _filePath.extension();
        LoadAssimp(_filePath);
    }
}