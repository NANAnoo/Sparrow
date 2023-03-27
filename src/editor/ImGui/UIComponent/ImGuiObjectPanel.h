/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#include "ImGuiInspectorPanel.h"
#include "ImGuiPanel.h"

namespace SPW {

class ImGuiObjectPanel : public ImGuiPanel
{
public:
  using MenuItemCallback = std::function<void()>;

  struct MenuItem
  {
    std::string label;
    MenuItemCallback callback;
  };

  ImGuiObjectPanel(const std::string& title, bool* open = nullptr)
      : ImGuiPanel(title, open)
  { }

  void AddMenuItem(const std::string& label, MenuItemCallback callback)
  {
    m_items.push_back({ label, std::move(callback) });
  }

  void SetMenuItems(const std::vector<std::shared_ptr<GameObject>>& gameObjects, const std::shared_ptr<ImGuiInspectorPanel>& panel)
  {
    ClearItems();

    for (const auto& gameObject : gameObjects)
    {
      AddMenuItem(gameObject->name, [&, gameObject]()
                  {
                    panel->SetSelectedGameObject(gameObject);
                  });
    }

  }

  void ClearItems()
  {
    m_items.clear();
  }

  void RemoveMenuItem(const std::string& label)
  {
    auto it = std::find_if(m_items.begin(), m_items.end(),
                           [&](const MenuItem& item) { return item.label == label; });

    if (it != m_items.end())
    {
      m_items.erase(it);
    }
  }

protected:
  void Draw() override
  {
    for (const auto& item : m_items)
    {
      if (ImGui::MenuItem(item.label.c_str()))
      {
        item.callback();
      }
    }
  }

private:
  std::vector<MenuItem> m_items;
};

}
