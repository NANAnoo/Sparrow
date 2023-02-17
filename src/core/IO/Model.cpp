/*
 * Created by elecfrog@du
 * 2023/02/16
 * Load .obj .gltf .fbx files
 * Stay Empty for API specification
 */
#include "Model.h"

namespace SPW
{
	Model::Model(const FilePath& _FilePath)
		:m_FilePath(_FilePath)
	{
		m_Meshes.resize(0);
		LoadModel(_FilePath);
	}

	Model::Model(const std::shared_ptr<Mesh>& mesh)
	{
		m_Meshes.emplace_back(mesh);
	}

	void Model::LoadModel(const FilePath& _filePath)
	{
		const auto& fileExtension = _filePath.extension();
		if (fileExtension == ".obj" || fileExtension == ".OBJ")
		{
			std::cout << "[Read From an OBJ File]" << _filePath << std::endl;
			LoadOBJ(_filePath);
		}
		else if (fileExtension == ".fbx" || fileExtension == ".FBX")
		{
			std::cout << "[Going To Support]" << std::endl;
			// LoadFBX(_filePath);
		}
		else if (fileExtension == ".gltf" || fileExtension == ".GLTF")
		{
			std::cout << "[Going To Support]" << std::endl;
			// LoadGLTF(_filePath);
		}
		else
		{
			// TODO: SPW_LOG_ERROR
			std::cout << "[Not Support Right Now]" << std::endl;
		}
	}


}
