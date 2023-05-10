#pragma once

#include "Pipeline.hpp"
#include "AttachmentTexture.h"
#include "FrameBuffer.h"
#include "Light.h"
#include "RenderBackEndI.h"
#include "Utils/UUID.hpp"
#include <sol/sol.hpp>

namespace SPW {
    const unsigned int SCREEN_PORT = -1;
    class AttachmentPort {
    public:
        AttachmentPort() = delete;
        AttachmentPort(const AttachmentPort &other) {
            pass_id = other.pass_id;
            resource_id = other.resource_id;
            screen_resource = other.screen_resource;
        }
        AttachmentPort&operator=(const AttachmentPort &other) {
            pass_id = other.pass_id;
            resource_id = other.resource_id;
            screen_resource = other.screen_resource;
            return *this;
        }

        bool operator==(const AttachmentPort &other) const {
            return pass_id == other.pass_id && resource_id == other.resource_id && screen_resource == other.screen_resource;
        }

        AttachmentPort(unsigned int pass_id, int resource_id) : pass_id(pass_id), resource_id(resource_id) {}
        AttachmentPort(unsigned int pass_id, ScreenAttachmentType screen_src) : pass_id(pass_id), screen_resource(screen_src) {}
        ~AttachmentPort() = default;

        unsigned int pass_id;
        int resource_id = -1;
        ScreenAttachmentType screen_resource = UnknownScreenAttachmentType;
    };

    struct AttachmentPortHash {
        std::size_t operator()(const AttachmentPort &port) const {
            return std::hash<unsigned int>()(port.pass_id) ^ 
                    (std::hash<int>()(port.resource_id) << 1) ^ 
                    (std::hash<ScreenAttachmentType>()(port.screen_resource) << 2);
        }
    };

    struct ShaderDesc {
    public:
        ShaderDesc() = default;
        ShaderDesc(const ShaderDesc &other) {
            shader = other.shader;
            mat_inputs = other.mat_inputs;
            transform_inputs = other.transform_inputs;
            context_inputs = other.context_inputs;
            light_inputs = other.light_inputs;
            dependency_inputs = other.dependency_inputs;
            uuid = other.uuid;
        }
        ShaderDesc&operator=(const ShaderDesc &other) {
            shader = other.shader;
            mat_inputs = other.mat_inputs;
            transform_inputs = other.transform_inputs;
            context_inputs = other.context_inputs;
            light_inputs = other.light_inputs;
            dependency_inputs = other.dependency_inputs;
            uuid = other.uuid;
            return *this;
        }
        UUID uuid = UUID::randomUUID();
        // --------------------- shader uniforms ---------------------
        // material uniforms
        std::unordered_map<MaterialType, std::string> mat_inputs;

        // transform uniforms
        std::unordered_map<TransformType, std::string> transform_inputs;

        // context uniforms
        std::unordered_map<ContextType, std::string> context_inputs;

        // light uniforms
        std::unordered_map<LightType, std::string> light_inputs;

        // attchment textures uniforms, from previous ModelSubPass
        std::unordered_map<AttachmentPort, std::string, AttachmentPortHash> dependency_inputs;

        // --------------------- shader uniforms ---------------------
        ShaderHandle shader;

        bool operator==(const ShaderDesc &other) const {
            return shader == other.shader;
        }
    };

    class RenderPassNodeI {
    public:
        explicit RenderPassNodeI() {
            static unsigned int self_increment_id = 0;
            pass_id = self_increment_id++;
        };

        RenderPassNodeI(RenderPassNodeI &&other)  noexcept {
            pass_id = other.pass_id;
            other.pass_id = 0;
            clearType = other.clearType;
            depthCompType = other.depthCompType;
            depthTest = other.depthTest;
            ready = other.ready;
            input_ports = std::move(other.input_ports);
        }

        RenderPassNodeI&operator=(RenderPassNodeI &&other)  noexcept {
            pass_id = other.pass_id;
            other.pass_id = 0;
            clearType = other.clearType;
            depthCompType = other.depthCompType;
            depthTest = other.depthTest;
            ready = other.ready;
            input_ports = std::move(other.input_ports);
            return *this;
        }

        virtual void updateFromLua(const std::string &key, const sol::table &table) {
            if (key == "clear_type") {
                clearType = ClearType(table.as<int>());
            } else if (key == "depth_comp") {
                depthCompType = DepthCompType(table.as<int>());
            } else if (key == "depth_test") {
                depthTest = table.as<bool>();
            }
        };

        virtual void bindInputPort(const AttachmentPort &port) {
            input_ports.push_back(port);
        }
        
        ClearType clearType = NoClear;
        DepthCompType depthCompType = DepthCompType::LESS_Type;
        bool depthTest = true;
        // check if all input ports are ready
        bool ready = false;

        unsigned int pass_id;
        std::vector<AttachmentPort> input_ports;
    };

    // model pass with inner attchements
    class ModelPassNode : public RenderPassNodeI {
    public:
        ModelPassNode() = delete;
        explicit ModelPassNode(AttachmentType type = ColorType) : output_type(type) {}
        virtual ~ModelPassNode() = default;

        // move constructor
        ModelPassNode(ModelPassNode &&other)  noexcept : RenderPassNodeI(std::move(other)) {
            output_type = other.output_type;
            attachment_formats = std::move(other.attachment_formats);
            frame_buffer_ref = other.frame_buffer_ref;
            width = other.width;
            height = other.height;
        }

        // move assignment
        ModelPassNode&operator=(ModelPassNode &&other)  noexcept {
            RenderPassNodeI::operator=(std::move(other));
            output_type = other.output_type;
            attachment_formats = std::move(other.attachment_formats);
            frame_buffer_ref = other.frame_buffer_ref;
            width = other.width;
            height = other.height;
            return *this;
        }
        
        void updateFromLua(const std::string &key, const sol::table &table) override {
            if (key == "width") {
                width = table.as<unsigned int>();
            } else if (key == "height") {
                height = table.as<unsigned int>();
            } else if (key == "attachment_type") {
                output_type = AttachmentType(table.as<int>());
            } else {
                RenderPassNodeI::updateFromLua(key, table);
            }
        }

        // inner attachments size
        unsigned int width = 0;
        unsigned int height = 0;

        AttachmentPort addAttachment(ColorAttachmentFormat format) {
            attachment_formats.push_back(format);
            return {pass_id, static_cast<int>(attachment_formats.size() - 1)};
        }

        AttachmentType output_type;
        std::vector<ColorAttachmentFormat> attachment_formats;

        int frame_buffer_ref = -1;
    };

    // execute multiple times to produce a texture array
    class ModelRepeatPassNode : public ModelPassNode {
    public:
        ModelRepeatPassNode() = delete;
        explicit ModelRepeatPassNode(AttachmentType type, RepeatType repeat, unsigned int count) : 
            ModelPassNode(type), repeat_type(repeat), repeat_count(count) {}
        
        // move constructor
        ModelRepeatPassNode(ModelRepeatPassNode &&other) : ModelPassNode(std::move(other)) {
            frame_buffer_refs = std::move(other.frame_buffer_refs);
            repeat_type = other.repeat_type;
            repeat_count = other.repeat_count;
        }

        // move assignment
        ModelRepeatPassNode&operator=(ModelRepeatPassNode &&other) {
            ModelPassNode::operator=(std::move(other));
            frame_buffer_refs = std::move(other.frame_buffer_refs);
            repeat_type = other.repeat_type;
            repeat_count = other.repeat_count;
            return *this;
        }

        void updateFromLua(const std::string &key, const sol::table &table) override {
            if (key == "repeat_type") {
                repeat_type = RepeatType(table.as<int>());
            } else if (key == "repeat_count") {
                repeat_count = table.as<unsigned int>();
            } else {
                ModelPassNode::updateFromLua(key, table);
            }
        }

        virtual ~ModelRepeatPassNode() = default;

        std::vector<unsigned int> frame_buffer_refs;
        RepeatType repeat_type = NoRepeat;
        unsigned int repeat_count = 0;
    };
    
    // model pass without inner attchements
    // output to screen buffer
    class ModelToScreenNode : public RenderPassNodeI {
    public:
        explicit ModelToScreenNode() = default;
        virtual ~ModelToScreenNode() = default;

        AttachmentPort addScreenAttachment(ScreenAttachmentType format) {
            screen_output_type = static_cast<ScreenAttachmentType>(screen_output_type | format);
            return {pass_id, format};
        }

        // move constructor
        ModelToScreenNode(ModelToScreenNode &&other)  noexcept : RenderPassNodeI(std::move(other)) {
            screen_output_type = other.screen_output_type;
        }

        // move assignment
        ModelToScreenNode&operator=(ModelToScreenNode &&other)  noexcept {
            RenderPassNodeI::operator=(std::move(other));
            screen_output_type = other.screen_output_type;
            return *this;
        }

        ScreenAttachmentType screen_output_type = UnknownScreenAttachmentType;
    };

    // present node
    class PresentNode : public RenderPassNodeI {
    public:
        PresentNode() = delete;
        explicit PresentNode(const ShaderDesc& shader) : shader(shader) {}

        // move constructor
        PresentNode(PresentNode &&other)  noexcept : RenderPassNodeI(std::move(other)) {
            shader = other.shader;
        }

        // move assignment
        PresentNode&operator=(PresentNode &&other)  noexcept {
            RenderPassNodeI::operator=(std::move(other));
            shader = other.shader;
            return *this;
        }

        unsigned int width = 0;
        unsigned int height = 0;

        virtual ~PresentNode() = default;

        ShaderDesc shader;  
    };

    // operate an image, work with a shader
     class ImagePassNode : public PresentNode {
     public:
         ImagePassNode() = delete;
         explicit ImagePassNode(ShaderDesc shader) : PresentNode(shader) {}
         ~ImagePassNode() override = default;

         // move constructor
         ImagePassNode(ImagePassNode &&other)  noexcept : PresentNode(std::move(other)) {
             attachment_formats = std::move(other.attachment_formats);
             frame_buffer_ref = other.frame_buffer_ref;
         }

         AttachmentPort addAttachment(ColorAttachmentFormat format) {
             attachment_formats.emplace_back(format);
             return {pass_id, static_cast<int>(attachment_formats.size() - 1)};
         }

         std::vector<ColorAttachmentFormat> attachment_formats;
          int frame_buffer_ref = -1;
     };

    // operate on screen buffer
    class ScreenPassNode : public PresentNode {
    public:
        ScreenPassNode() = delete;
        explicit ScreenPassNode(const ShaderDesc& shader) : PresentNode(shader) {}
        ~ScreenPassNode() override = default;

        // move constructor
        ScreenPassNode(ScreenPassNode &&other)  noexcept : PresentNode(std::move(other)) {
            screen_output_type = other.screen_output_type;
        }

        // move assignment
        ScreenPassNode&operator=(ScreenPassNode &&other) {
            PresentNode::operator=(std::move(other));
            screen_output_type = other.screen_output_type;
            return *this;
        }

        AttachmentPort addScreenAttachment(ScreenAttachmentType format) {
            screen_output_type = static_cast<ScreenAttachmentType>(screen_output_type | format);
            return {pass_id, format};
        }

        ScreenAttachmentType screen_output_type = UnknownScreenAttachmentType; 
    };

    enum class MeshSourceType {
        MeshFromAsset,
        MeshFromUIStorage
    };

    class Entity;
    struct RenderInput {
        MeshSourceType sourceType = MeshSourceType::MeshFromAsset;
        unsigned int screen_width;
        unsigned int screen_height;
        std::unordered_map<unsigned int, std::vector<Entity>> render_models;
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 camera_pos;
        glm::vec3 camera_dir;
        std::vector<PLight> pLights;
        std::vector<DLight> dLights;
        std::shared_ptr<FrameBuffer> screen_buffer;
        std::shared_ptr<AttachmentTexture> screen_texture;
        std::shared_ptr<AttachmentTexture> depth_texture;
        std::shared_ptr<RenderBackEndI> backend;
        std::unordered_map<UUID, ShaderDesc, UUID::hash> shader_map;
    };

    template <class T>
    concept RenderNode = std::is_base_of_v<RenderPassNodeI, T>;

    class RenderGraph {
    public:
        RenderGraph() = default;
        ~RenderGraph() = default;

        // create a model pass node and return it as shared_ptr
        template<RenderNode T, typename...Args>
        std::shared_ptr<T> createRenderNode(Args ... args) {
            auto node = std::make_shared<T>(args...);
            all_nodes.insert({node->pass_id, node});
            return node;
        }

        // init with a backend
        virtual void init(unsigned int s_width, unsigned int s_height) = 0;

        virtual void onFrameChanged(unsigned int s_width, unsigned int s_height) = 0;

        // render
        virtual void render(const RenderInput &input) = 0;

        // id
        unsigned int graph_id = 0;
    protected:
        // all nodes
        std::unordered_map<unsigned int, std::shared_ptr<RenderPassNodeI>> all_nodes;
    };

}