/*
 * @date   2023-04-15
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiEntityPanel.h"

namespace SPW
{
	void ImGuiEntityPanel::UpdateMenuItemLabel(const std::string& id, const std::string& label)
	{
		auto it = m_Items.find(id);
		if (it != m_Items.end())
		{
			it->second.name = label;
		}
	}

	void ImGuiEntityPanel::AddMenuItem(const std::string& id, const std::string& label, MenuItemCallback callback)
	{
		UpdateMenuItemLabel(id, label);
		if (!m_Items.contains(id))
		{
			m_Items[id] = { id, label, std::move(callback)};
		}
	}

	void ImGuiEntityPanel::RemoveMenuItem(const std::string& ID)
	{
		const auto it = m_Items.find(ID);

		if (it != m_Items.end())
		{
			m_Items.erase(it);
		}
	}

	void ImGuiEntityPanel::Draw()
	{
		for (const auto& item_pair : m_Items)
		{
			const auto& item = item_pair.second;
			// std::stringstream ss;
			// ss << "x##" << item_pair.first;
			std::string button_label = "x##" + item_pair.first;
			if (ImGui::Button(button_label.c_str()))
			{
				m_ScenePtr->deleteEntity(m_ScenePtr->getEntityByID(item_pair.first));
				RemoveMenuItem(item_pair.first);
			}
			ImGui::SameLine();
			bool is_selected = false;
			ImVec2 item_min = ImGui::GetItemRectMin();
			ImVec2 item_max = ImGui::GetItemRectMax();
			ImGui::Selectable((ICON_FA_CUBE"		"+item.name).c_str(), &is_selected, ImGuiSelectableFlags_None, ImVec2(0, item_max.y - item_min.y));
			if (ImGui::IsItemClicked())
			{
				item.callback();
			}
		}


		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Add Entity").x) * 0.5f);
		if( ImGui::Button( "Add Entity" ) )
		{
			auto new_gameObject = m_ScenePtr->createEntity("NewGameObject");
			new_gameObject->emplace<TransformComponent>();
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Load Entity").x) * 0.5f);
		if (ImGui::Button("Load Entity"))
		{
			std::string defaultPath;
			if (std::filesystem::exists(Config::k_WorkingProjectAssets + "Prefabs/"))
			{
				defaultPath = Config::k_WorkingProjectAssets + "Prefabs/";
			}
			else
			{
				defaultPath = Config::k_WorkingProjectAssets;
			}

			// ".*,.cpp,.h,.hpp" => simple filters
			m_FileDialog->OpenDialog("LoadEntity", "Load Entity", ".*,.prefab,.json", defaultPath.c_str());
			// EntitySerializer::LoadEntity(m_ScenePtr);
		}

		if(m_FileDialog->Display("LoadEntity"))
		{
			if( m_FileDialog->IsOk() )
			{
				std::string filePathName = m_FileDialog->GetFilePathName();
				EntitySerializer::LoadEntity(m_ScenePtr, filePathName);
			}
			m_FileDialog->Close();
		}

	}
}
