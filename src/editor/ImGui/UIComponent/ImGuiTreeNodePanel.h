/*
 * @date   2023-03-27
 * @author dudu
 * @brief  ${FILE_DESCRIPTION}
 */
#pragma once
#include "ImGuiPanel.h"

namespace SPW
{

class ImGuiTreeNodePanel : public ImGuiPanel
{
public:
  using TreeNodeCallback = std::function<void()>;

  struct TreeNode
  {
    std::string label;
    std::vector<TreeNode> children;
    TreeNodeCallback callback;
  };

  ImGuiTreeNodePanel(std::string title, bool* open = nullptr)
      : ImGuiPanel( std::move(title), open)
  { }

  ImGuiTreeNodePanel(std::string title, std::vector<TreeNode> treeNodes, bool* open = nullptr)
      : ImGuiPanel(std::move(title), open)
        , m_treeNodes(std::move(treeNodes))
  { }

  void AddTreeNode(const std::string& label, TreeNodeCallback callback)
  {
    m_treeNodes.emplace_back( TreeNode{ label, {}, std::move(callback) } );
  }

  void AddChildTreeNode(const std::string& parentLabel, const std::string& label, TreeNodeCallback callback)
  {
    for (auto& node : m_treeNodes)
    {
      if (node.label == parentLabel)
      {
        node.children.emplace_back( TreeNode{ label, {}, std::move(callback) } );
        break;
      }
    }
  }

protected:
  void Draw() override
  {
    RecursiveDrawTreeNodes(m_treeNodes);
  }

private:
  void RecursiveDrawTreeNodes(const std::vector<TreeNode>& nodes)
  {
    for (const auto& node : nodes)
    {
      if (ImGui::TreeNode(node.label.c_str()))
      {
        if (node.callback && ImGui::IsItemClicked())
        {
          node.callback();
        }
        RecursiveDrawTreeNodes(node.children);
        ImGui::TreePop();
      }
    }
  }

  std::vector<TreeNode> m_treeNodes;
};

}
