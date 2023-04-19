#pragma once
#include "Render/RenderGraph.hpp"
#include "Utils/UUID.hpp"

#include <string>
#include <unordered_map>
#include <sol/sol.hpp>

namespace SPW {
    class ShaderDescWrapper {
    public:
        ShaderDescWrapper() = default;
        std::string getID() const { return m_desc.uuid.toString(); }

        void setMatInput(int type, const std::string &value) {
            m_desc.mat_inputs[static_cast<MaterialType>(type)] = value;
        }
        
        void setTransformInput(int type, const std::string &value) {
            m_desc.transform_inputs[static_cast<TransformType>(type)] = value;
        }

        void setContextInput(int type, const std::string &value) {
            m_desc.context_inputs[static_cast<ContextType>(type)] = value;
        }

        void setLightInput(int type, const std::string &value) {
            m_desc.light_inputs[static_cast<LightType>(type)] = value;
        }

        void setDependencyInput(const AttachmentPort &port, const std::string &value) {
            m_desc.dependency_inputs[port] = value;
        }

        void setShader(const ShaderHandle &shader) {
            m_desc.shader = shader;
        }
        
        static void bindShaderDescWrapper(sol::table &parent) {
            parent.new_usertype<ShaderDescWrapper>("ShaderDescWrapper",
                "getID", &ShaderDescWrapper::getID,
                "setMatInput", &ShaderDescWrapper::setMatInput,
                "setTransformInput", &ShaderDescWrapper::setTransformInput,
                "setContextInput", &ShaderDescWrapper::setContextInput,
                "setLightInput", &ShaderDescWrapper::setLightInput,
                "setDependencyInput", &ShaderDescWrapper::setDependencyInput,
                "setShader", &ShaderDescWrapper::setShader
            );

            parent["createShaderDesc"] = []() {
                return ShaderDescWrapper();
            };
        }
        ShaderDesc m_desc{};
    };

    class RenderNodeWrapper {
    public:
        RenderNodeWrapper() = delete;
        RenderNodeWrapper(const std::shared_ptr<RenderPassNodeI> &node) : m_node(node) {}

        unsigned int getID() const { return m_node.lock()->pass_id;}

        void bindInputPort(const AttachmentPort &port) {
            m_node.lock()->bindInputPort( port);
        }

        AttachmentPort addAttachment(int attachment_format) {
            ColorAttachmentFormat format = ColorAttachmentFormat(attachment_format);
            return std::dynamic_pointer_cast<ModelPassNode>(m_node.lock())->addAttachment(format);
        }

        AttachmentPort setScreenOutputType(int attachment_format) {
            ScreenAttachmentType format = ScreenAttachmentType(attachment_format);
            if (std::dynamic_pointer_cast<ModelToScreenNode>(m_node.lock()))
                return std::dynamic_pointer_cast<ModelToScreenNode>(m_node.lock())->addScreenAttachment(format);
            return std::dynamic_pointer_cast<ScreenPassNode>(m_node.lock())->addScreenAttachment(format);
        }

        void update(const std::string &key, const sol::table &value) {
            m_node.lock()->updateFromLua(key, value);
        }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<RenderNodeWrapper>("RenderNodeWrapper",
                "getID", &RenderNodeWrapper::getID,
                "bindInputPort", &RenderNodeWrapper::bindInputPort,
                "addAttachment", &RenderNodeWrapper::addAttachment,
                "setScreenOutputType", &RenderNodeWrapper::setScreenOutputType,
                "update", &RenderNodeWrapper::update
            );
        }
    private:
        std::weak_ptr<RenderPassNodeI> m_node;
    };

    class RenderGraphWrapper {
    public:
        RenderGraphWrapper() = delete;
        RenderGraphWrapper(const std::shared_ptr<RenderGraph>& graph) : m_graph(graph) {}

        RenderNodeWrapper createRenderNode(const std::string &type, const sol::table &value) {
            ClearType clear_type = ClearType(value.get<int>("clear_type"));
            DepthCompType depth_comp = DepthCompType(value.get<int>("depth_comp"));
            bool depth_test = value["depth_test"];
            std::shared_ptr<RenderPassNodeI> node = nullptr;
            if (type == "ModelPassNode") {
                AttachmentType attachment_type = AttachmentType(value.get<int>("attachment_type"));
                unsigned int width = value["width"];
                unsigned int height = value["height"];
                auto tmp_node = m_graph.lock()->createRenderNode<ModelPassNode>(attachment_type);
                tmp_node->height = height;
                tmp_node->width = width;
                node = tmp_node;
            } else if (type == "ModelRepeatPassNode") {
                AttachmentType attachment_type = AttachmentType(value.get<int>("attachment_type"));
                RepeatType repeat_type = RepeatType(value.get<int>("repeat_type"));
                int repeat_count = value["repeat_count"];
                unsigned int width = value["width"];
                unsigned int height = value["height"];
                auto tmp_node = m_graph.lock()->createRenderNode<ModelRepeatPassNode>(attachment_type, repeat_type, repeat_count);
                tmp_node->height = height;
                tmp_node->width = width;
                node = tmp_node;
            } else if (type == "ModelToScreenNode") {
                node = m_graph.lock()->createRenderNode<ModelToScreenNode>();
            } else if (type == "PresentNode") {
                ShaderDescWrapper shader_desc = value["shader_desc"]["cpp_object"];
                node = m_graph.lock()->createRenderNode<PresentNode>(shader_desc.m_desc);
            } else if (type == "ScreenPassNode") {
                ShaderDescWrapper shader_desc = value["shader_desc"]["cpp_object"];
                node = m_graph.lock()->createRenderNode<ScreenPassNode>(shader_desc.m_desc);
            }
            return RenderNodeWrapper(node);
        }

        unsigned int getID() const { return m_graph.lock()->graph_id; }

        static void bindLuaTable(sol::table &parent) {
            parent.new_usertype<RenderGraphWrapper>("RenderGraphWrapper",
                "createRenderNode", &RenderGraphWrapper::createRenderNode,
                "getID", &RenderGraphWrapper::getID
            );
        }

    private:
        std::weak_ptr<RenderGraph> m_graph;
    };

    static void bindAttachmentPort(sol::table &parent) {
        parent.new_usertype<AttachmentPort>("AttachmentPort",
            sol::call_constructor, sol::constructors<SPW::AttachmentPort(unsigned int, int)>(),
            "pass_id", &AttachmentPort::pass_id,
            "screen_resource", &AttachmentPort::screen_resource,
            "resource_id", &AttachmentPort::resource_id
        );
    } 
};