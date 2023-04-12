#pragma once
#include "../ImGuiDefinitions.h"
#include "ImGuiPanel.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include <ostream>
#include <iostream>

#include "Asset/ModelLoader/ModelLoader.h"
#include "Asset/AssetManager/AssetManager.h"
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

                	std::string extension = FileSystem::ToFsPath(fileName).extension().string();
                    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                    if (extension == ".gltf" || extension == ".fbx" || extension == ".obj")
                    {
                        if (AssetManager::ImportModel(filePathName))
                        {
                            ImGui::OpenPopup("Import Model");
                        }
                    }
                    else
                    {
                        ImGui::OpenPopup("Import Model");
                    }

                    if (ImGui::BeginPopupModal("Import Model", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        if (extension == ".gltf" || extension == ".fbx" || extension == ".obj")
                        {
                            ImGui::Text("Import Model Sucessed!");
                        }
                        else
                        {
                            ImGui::Text("Dude, not supported this format!");
                        }
                        if (ImGui::Button("x")) { ImGui::CloseCurrentPopup(); }
                        ImGui::EndPopup();
                    }

                }
                file_dialog->Close();
            }
        }

    private:
        SharedPtr<ImGuiFileDialog> file_dialog;
    };

}
