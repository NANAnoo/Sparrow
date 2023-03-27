/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"

namespace SPW {

class ImGuiMenuBar : public ImGuiPanel {
public:
  using MenuItemCallback = std::function<void()>;

  struct MenuItem {
    std::string label;
    MenuItemCallback callback;
  };

  struct SubMenu {
    std::string label;
    std::vector<MenuItem> items;
  };

  ImGuiMenuBar(const std::string &title, bool *open = nullptr)
      : ImGuiPanel(title, open) {}

  void AddSubMenu(const std::string &label) {
    m_subMenus.emplace_back(SubMenu{label, {}});
  }

  void AddMenuItemToSubMenu(const std::string &subMenuLabel,
                            const std::string &itemLabel,
                            MenuItemCallback callback) {
    for (auto &subMenu : m_subMenus) {
      if (subMenu.label == subMenuLabel) {
        subMenu.items.emplace_back(MenuItem{itemLabel, callback});
        break;
      }
    }
  }

protected:
  void Begin() override { ImGui::BeginMainMenuBar(); }

  void Draw() override {
    for (const auto &subMenu : m_subMenus) {
      if (ImGui::BeginMenu(subMenu.label.c_str())) {
        for (const auto &menuItem : subMenu.items) {
          if (ImGui::MenuItem(menuItem.label.c_str())) {
            menuItem.callback();
          }
        }
        ImGui::EndMenu();
      }
    }
  }

  void End() override { ImGui::EndMainMenuBar(); }

private:
  std::vector<SubMenu> m_subMenus;
};

}