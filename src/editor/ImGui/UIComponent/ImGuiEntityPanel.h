/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiInspectorPanel.h"
#include "ImGuiPanel.h"

namespace SPW {

class ImGuiEntityPanel : public ImGuiPanel
{
public:
	using MenuItemCallback = std::function<void()>;

	struct MenuItem
	{
		std::string ID;
		std::string name;
		MenuItemCallback callback;
	};

	ImGuiEntityPanel(const std::string& title, bool* open = nullptr)
		: ImGuiPanel(title, open)
	{ }

	void AddMenuItem(const std::string& id, const std::string& label, MenuItemCallback callback)
	{
	  if (m_Items.find(id) == m_Items.end())
	  {
	      m_Items[id] = { id, label, std::move(callback) };
	  }
	}

	void ClearItems()
	{
		m_Items.clear();
	}

  void RemoveMenuItem(const std::string& ID)
  {
	  auto it = m_Items.find(ID);

	  if (it != m_Items.end())
	  {
		  m_Items.erase(it);
	  }
  }

protected:
  void Draw() override
  {
	  for (const auto& item_pair : m_Items)
	  {
		  const auto& item = item_pair.second;
		  if (ImGui::MenuItem(item.name.c_str()))
		  {
			  item.callback();
              //std::cout<<"clicked"<<std::endl;
		  }
	  }
  }

private:
  std::unordered_map<std::string, MenuItem> m_Items;
};

}
