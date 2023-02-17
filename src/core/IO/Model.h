/*
 * Created by elecfrog@du
 * 2023/02/16
 * Load .obj .gltf .fbx files
 * Stay Empty for API specification
 */

#pragma once

#include "IO/Mesh.h"
#include "IO/Material.h"
#include "Utils/MacroUtils.h"

namespace SPW
{
    class Model
    {
    public:
        Model() = default;
        explicit Model(const FilePath& _filePath);
        // TODO: Add PrimitiveType type
        Model(const SharedPtr<Mesh>& _meshes);

        ~Model() = default;

        std::vector<SharedPtr<Mesh>>& GetMeshes() { return m_Meshes; }
        const std::vector<SharedPtr<Mesh>>& GetMeshes() const { return m_Meshes; }
        void AddMesh(SharedPtr<Mesh> _mesh) { m_Meshes.emplace_back(std::move(_mesh)); }

        // TODO: Save Methods, Load Methods
        void LoadModel(const FilePath& _filePath);

    private:
        // TODO: Add PrimitiveType type
        std::vector<SharedPtr<Mesh>>  m_Meshes;
        FilePath m_FilePath;

        void LoadOBJ(const FilePath& _filePath);
        // TODO: Support
        // void LoadFBX(const char* _filePath);
        // void LoadGLTF(const char* _filePath);

    };
}

