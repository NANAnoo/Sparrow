#pragma once
#include "../ImGuiDefinitions.h"
#include "ImGuiPanel.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include <ostream>
#include <iostream>

#include "Asset/ModelLoader/ModelLoader.h"
#include "Asset/AssetManager/AssetManager.h"
#include "ImGui/ImGuiFileDialog.h"
#include "ImGuiMessageBox/ImGuiMessageBox.h"
#include "ImGui/ImGuiIconManager.hpp"

namespace SPW
{

    class ImGuiFileDialogPanel : public ImGuiPanel
    {
    public:
        ImGuiFileDialogPanel()
            : ImGuiPanel("File Dialog Panel")
        {
	        file_dialog = std::make_shared<ImGuiFileDialog>();

        	importModel_MessageBox = std::make_unique<ImGuiMessageBox>("Import Model", "file", "Import Model Sucessed!", std::vector{ "OK" }, false);
            textureCompression_MessageBox = std::make_unique<ImGuiMessageBox>("textureCompression_MessageBox Model", "file", "textureCompression_MessageBox Model Sucessed!", std::vector{ "OK" }, false);
        }

        void Draw() override
    	{
            // importModel_MessageBox->SetTrigger();
            // if (ImGui::Button("Open File Dialog")) 
            // {
            //     file_dialog->OpenDialog("ChooseFileDlgKey", "Choose File", "*.*", ".");
            // }
            //
            // if (file_dialog->Display("ChooseFileDlgKey"))
            // {
            // 	if (file_dialog->IsOk())
            //     {
            //
            // 		std::string file_path = file_dialog->GetFilePathName();
            //         // Do something with the file_path
            //         std::string filePathName = file_dialog->GetFilePathName();
            //         std::string fileName = file_dialog->GetCurrentFileName();
            //
            //         std::string extension = FileSystem::ToFsPath(fileName).extension().string();
            //
            //         std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            //         if (extension == ".gltf" || extension == ".fbx" || extension == ".obj")
            //         {
            //             if (AssetManager::ImportModel(filePathName))
            //             {
            //                 importModel_MessageBox->trigger_flag = true;
            //             }
            //         }
            //
            //     }
            //     file_dialog->Close();
            // }
            // if (importModel_MessageBox->trigger_flag) { importModel_MessageBox->Exec(); }


            textureCompression_MessageBox->SetTrigger();
            // --------------------- TRIGGER------------------
            
            if (ImGui::Button("Image Compression"))
            {
                file_dialog->OpenDialog("ChooseFileDlgKey2", "Choose File", "*.*", ".");
            }
            if (file_dialog->Display("ChooseFileDlgKey2"))
            {
                if (file_dialog->IsOk())
                {
            
                    std::string file_path = file_dialog->GetFilePathName();
                    // Do something with the file_path
                    std::string filePathName = file_dialog->GetFilePathName();
                    std::string fileName = file_dialog->GetCurrentFileName();
            
                    std::string extension = FileSystem::ToFsPath(fileName).extension().string();
            
                    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                    if (extension == ".png" || extension == ".jpg" || extension == ".jepg")
                    {
                        auto data = ImGuiIconManager::LoadTextureData(filePathName);
                        if (ImGuiIconManager::ImageCompression(std::move(data), filePathName))
                        {
                            textureCompression_MessageBox->trigger_flag = true;
                        }
                    }
                }
                file_dialog->Close();
            }
            if (textureCompression_MessageBox->trigger_flag) { textureCompression_MessageBox->Exec(); }


        }

    private:
        SharedPtr<ImGuiFileDialog> file_dialog;
        std::unique_ptr<ImGuiMessageBox> importModel_MessageBox;
        std::unique_ptr<ImGuiMessageBox> textureCompression_MessageBox;
        // uint32_t trigger_flag = 0;
    };

}
