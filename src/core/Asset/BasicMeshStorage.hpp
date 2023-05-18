//
// Created by NANAnoo on 5/10/2023.
//

#pragma once
#include "Utils/SingletonBase.h"
#include "Utils/UUID.hpp"
#include "AssetData/MaterialData.h"

namespace SPW {
    enum BasicMeshType {
        UIMesh
    };

    class BasicMeshBuilder {
    public:
        std::function<void(const std::shared_ptr<Mesh> &mesh)> addMesh = nullptr;
        std::function<void(const MaterialData &material)> addMaterial = nullptr;
        std::function<void(const std::string &, const UUID &)> addTexture = nullptr;
    };

    template <BasicMeshType type>
    class BasicMeshStorage : public SingletonBase<BasicMeshStorage<type>> {
    public:
        BasicMeshBuilder insert(const std::string &key) {
            BasicMeshBuilder builder;
            builder.addMesh = [this, key](const std::shared_ptr<Mesh> &mesh){
                auto &meshes = all_meshes[key];
                meshes.emplace_back(mesh);
            };
            builder.addMaterial = [this, key](const MaterialData &material){
                auto &materials = all_materials[key];
                materials.insert({material.ID, material});
            };
            builder.addTexture = [this, key](const std::string &path, const UUID &id){
                auto &textures = all_textures[key];
                textures[id.toString()] = path;
            };
            return builder;
        }
        using MeshCallBack = std::function<void(const std::shared_ptr<Mesh>&, const MaterialData&, const std::unordered_map<std::string, std::string>&)>;
        void forEachMesh(const std::string &key,
                         const MeshCallBack &callback) {
            if (all_meshes.contains(key)) {
                for (auto &mesh : all_meshes.at(key)) {
                    MaterialData mat;
                    if (all_materials.contains(key) && all_materials[key].contains(mesh->materialID)) {
                        mat = all_materials[key][mesh->materialID];
                    }
                    callback(mesh, mat, all_textures[key]);
                }
            }
        }
    private:
        std::unordered_map<std::string, std::vector<std::shared_ptr<Mesh>>> all_meshes;
        std::unordered_map<std::string, std::unordered_map<std::string, MaterialData>> all_materials;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> all_textures;
    };
};
