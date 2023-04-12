#pragma once
#include "../ImGuiDefinitions.h"
#include "ImGuiPanel.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include <ostream>
#include <iostream>

#include "Asset/ModelLoader/ModelLoader.h"
#include "ImGui/ImGuiFileDialog.h"

namespace SPW
{

    class ImGuiFileDialogPanel : public ImGuiPanel
    {
    public:
        ImGuiFileDialogPanel()
            : ImGuiPanel("File Dialog Panel")
        {
	        file_dialog = std::make_shared<ImGuiFileDialog>();
        }

        void Draw() override{
            if (ImGui::Button("Open File Dialog")) {
                file_dialog->OpenDialog("ChooseFileDlgKey", "Choose File", "*.*", ".");
            }

            if (file_dialog->Display("ChooseFileDlgKey")) {
                if (file_dialog->IsOk()) {
                    std::string file_path = file_dialog->GetFilePathName();
                    // Do something with the file_path
                    std::string filePathName = file_dialog->GetFilePathName();
                    std::string fileName = file_dialog->GetCurrentFileName();

                    std::string fileExt = filePathName.substr(filePathName.find_last_of('.') + 1);
                    std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);
                    if (fileExt == "gltf")
                    {
                        // 如果选中的文件是gltf文件，则调用SPW::ModelLoader::LoadModel(path)
                        ImportModel(filePathName);
                    }

                }
                file_dialog->Close();
            }
        }

        void ImportModel(const std::string& path)
        {
            // assume load two models into the scene
            std::unique_ptr<SPW::ModelData> model_0 = SPW::ModelLoader::LoadModel(path);

            std::string absolute_modelDir = SPW::FileSystem::JoinPaths(SPW::FileRoots::k_Assets, model_0->name);
            SPW::FileSystem::CreateDirectory(absolute_modelDir);

            // Update Model Path
            model_0->path = SPW::FileSystem::JoinPaths(absolute_modelDir, model_0->name) + ".json";

            // write file with absolute file path
            std::ofstream of_file(model_0->path); // TODO .asset, use json to view the file currently
            cereal::JSONOutputArchive ar(of_file);

            // copy texture files into /Assets/Textures...
            // 1. Create Texture Directory
            std::string model_texture_dir = SPW::FileSystem::JoinPaths(absolute_modelDir, "Textures/");
            SPW::FileSystem::CreateDirectory(model_texture_dir);
            // 2. Loop Textures and Copy Files into Texture Directory
            for (auto& [k, v] : model_0->textures)
            {
                std::string destinationFilePath(model_texture_dir + SPW::FileSystem::ToFsPath(v).filename().string());
                SPW::FileSystem::CopyFile(v, destinationFilePath);

                // 3. Update Texture Path To Relative
                v = SPW::FileSystem::ToRelativePath(destinationFilePath, SPW::FileRoots::k_Root);
            }

            ar(
                // cereal::make_nvp("assetType", model_0->type),
                cereal::make_nvp("assetID", model_0->assetID),
                cereal::make_nvp("assetPath", SPW::FileSystem::ToRelativePath(model_0->path, SPW::FileRoots::k_Root)),

                // cereal::make_nvp("assetName", model_0->name),
                cereal::make_nvp("meshURI", model_0->meshURI),
                cereal::make_nvp("materials", model_0->materials),
                cereal::make_nvp("textures", model_0->textures)
            );

            {
                std::ofstream mesh_bin(SPW::FileSystem::JoinPaths(absolute_modelDir, model_0->meshURI) + ".mesh", std::ios::binary);
                cereal::BinaryOutputArchive archive(mesh_bin);
                archive(cereal::make_nvp(model_0->meshURI, model_0->meshes));
            }
        }



    private:
        SharedPtr<ImGuiFileDialog> file_dialog;
    };

}
