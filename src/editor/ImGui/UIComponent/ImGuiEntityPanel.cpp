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

	void ImGuiEntityPanel::AddMenuItem(const std::string& id, const std::string& label, MenuItemCallback callback, bool selected)
	{
		UpdateMenuItemLabel(id, label);
		if (!m_Items.contains(id))
		{
			m_Items[id] = { id, label, std::move(callback), selected};
		}
	}
}