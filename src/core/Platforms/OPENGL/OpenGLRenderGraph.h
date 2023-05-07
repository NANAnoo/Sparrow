#include "Platforms/OPENGL/OpenGLAttachmentTexture.hpp"
#include "Platforms/OPENGL/OpenGLFrameBuffer.h"
#include "Render/RenderGraph.hpp"


namespace SPW {
    class OpenGLRenderGraph : public RenderGraph {
        public:
            explicit OpenGLRenderGraph()=default;

            // create resource for pass node
            void init(unsigned int s_width, unsigned int s_height) override;
            void onFrameChanged(unsigned int s_width, unsigned int s_height) override;
            void render(const RenderInput &input) override;

        private:
            using ModelNodeMap = std::unordered_map<unsigned int, std::unordered_map<UUID, std::vector<Entity>, UUID::hash>>;
            int output_node_id = -1;
            // all frame buffers
            std::vector<std::shared_ptr<OpenGLFrameBuffer>> all_frame_buffers;

            // all attachments, four types
            std::unordered_map<AttachmentPort, std::shared_ptr<OpenGLAttachmentTexture>, AttachmentPortHash> all_attachments;
            std::unordered_map<AttachmentPort, std::shared_ptr<OpenGLAttachmentTextureArray>, AttachmentPortHash> all_attachment_arrays;
            std::unordered_map<AttachmentPort, std::shared_ptr<OpenGLAttachmentTextureCube>, AttachmentPortHash> all_attachment_cubes;
            std::unordered_map<AttachmentPort, std::shared_ptr<OpenGLAttachmentTextureCubeArray>, AttachmentPortHash> all_attachment_cube_arrays;

            // attachment map

            // create resource for model node
            void createResourceForModelNode(const std::shared_ptr<ModelPassNode> &node);

            // create resource for model repeat node
            void createResourceForModelRepeatNode(const std::shared_ptr<ModelRepeatPassNode> &node);

            // create resource for image pass node
            void createResourceForImagePassNode(const std::shared_ptr<ImagePassNode> &node, unsigned int s_width, unsigned int s_height);

            // bind required attachments
            unsigned int bindAttachments(
                const std::shared_ptr<Shader> &shader, 
                const ShaderDesc &desc, 
                const std::shared_ptr<AttachmentTexture> &screen_color, 
                const std::shared_ptr<AttachmentTexture> &screen_depth, 
                unsigned int slot);
            // render on node
            void renderOnNode(unsigned int, const ModelNodeMap& map, const RenderInput &input);

            // render on model pass node
            void renderOnModelPassNode(const std::shared_ptr<ModelPassNode> &node, const ModelNodeMap& map, const RenderInput &input);

            // render on model repeat pass node
            void renderOnModelRepeatPassNode(const std::shared_ptr<ModelRepeatPassNode> &node, const ModelNodeMap& map, const RenderInput &input);

            // render on model to screen pass node
            void renderOnModelToScreenNode(const std::shared_ptr<ModelToScreenNode> &node, const ModelNodeMap& map, const RenderInput &input);

            // render on screen pass node
            void renderOnScreenPassNode(const std::shared_ptr<ScreenPassNode> &node, const RenderInput &input);

            // render on image pass node
            void renderOnImagePassNode(const std::shared_ptr<ImagePassNode> &node, const RenderInput &input);

            // render on present node
            void renderOnPresentNode(const std::shared_ptr<PresentNode> &node, const RenderInput &input);

            // drawWithModels
            void drawWithModels(const RenderInput &input, const std::unordered_map<UUID, std::vector<Entity>, UUID::hash> &models, RepeatType type, unsigned int repeat_id, int face);
    };
}