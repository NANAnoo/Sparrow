/*
 * @date   2023-04-15
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGui/UIComponent/ImGuiPanel.h"
#include "Asset/ResourceManager/ResourceManager.h"

namespace SPW
{

	class ImGuiProfilingPanel : public ImGuiPanel
	{
	public:
		ImGuiProfilingPanel()
			: ImGuiPanel("Profiling Panel")
		{ }

		void Draw() override
		{
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Separator();

			ImGui::Text( "Loaded Models" );
			uint64_t drawCall_count = 0;
			uint64_t texture_count  = 0;
			for(const auto&[k, v]: ResourceManager::getInstance()->m_AssetDataMap)
			{
				if(!k.empty())
				{
					if (ImGui::CollapsingHeader(k.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("id:   %s", v.assetID.c_str());
						ImGui::Text("path: %s", v.path.c_str());
					}
					for (const auto& mesh : v.meshes)
					{
						drawCall_count += mesh.vertices.size();
					}
					drawCall_count /= 3;
					texture_count += v.textures.size();

				}
			}

			ImGui::Separator();
			ImGui::Text( "Draw Call: %llu", drawCall_count);

			ImGui::Separator();
			ImGui::Text( "Texture Count: %llu", texture_count);
		}
	private:
	};

}
