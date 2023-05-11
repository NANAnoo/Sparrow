#pragma once

#include "EcsFramework/Entity/Entity.hpp"
#include "Render/RenderBackEndI.h"
#include "Render/RenderGraph.hpp"

#include "EcsFramework/System/SystemI.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"
#include "Platforms/OPENGL/OpenGLAttachmentTexture.hpp"
#include "Asset/ResourceManager/ResourceManager.h"
#include "EcsFramework/Component/MeshComponent.hpp"
#include "Render/DefaultRenderPass.hpp"

namespace SPW {

    class SPWRenderSystem : public SystemI, public WindowEventResponder {
    public:
        explicit SPWRenderSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<RenderBackEndI> backEnd, int w, int h)
			: SystemI(scene)
			, renderBackEnd(std::move(backEnd))
			, WindowEventResponder(std::dynamic_pointer_cast<EventResponderI>(scene))
			{
                width = w;
                height = h;


                postProcessGraph = renderBackEnd->createRenderGraph();
                presentNode = postProcessGraph->createRenderNode<SPW::PresentNode>(SPW::FXAA_desc({SPW::SCREEN_PORT, SPW::ScreenColorType}));
                presentNode->bindInputPort({SPW::SCREEN_PORT, SPW::ScreenColorType});
                presentNode->depthTest = false;

                uiGraph = renderBackEnd->createRenderGraph();
                uiNode = uiGraph->createRenderNode<SPW::ModelToScreenNode>();
                uiNode->addScreenAttachment(SPW::ScreenColorType);
                uiNode->clearType = SPW::ClearType::ClearDepth;
                uiGraph->graph_id = 777;

                UIProgram = UIShader();
                addShaderDescriptor(UIProgram);
            }
        void setupRenderBackEnd(const std::shared_ptr<RenderBackEndI> &backEnd) {
            renderBackEnd = backEnd;
        };

        using RenderableEntity = std::tuple<IDComponent*, MeshComponent*, TransformComponent*>;
        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final {}
        void afterUpdate() final;
        void onStop() final;
        bool onFrameResize(int w, int h) override;

        void addShaderDescriptor(const ShaderDesc& desc) {
            shader_storage.insert({desc.uuid, desc});
        }
        // events
        const char *getName() override {return "SPW_RENDER_SYSTEM";}

        [[nodiscard]] inline GLuint getTextureID() const {
            return std::dynamic_pointer_cast<OpenGLAttachmentTexture>(screenTexture)->m_textureID;
        }

        std::shared_ptr<RenderGraph> postProcessGraph;
        std::shared_ptr<PresentNode> presentNode;

        std::shared_ptr<RenderGraph> uiGraph;
        std::shared_ptr<ModelToScreenNode> uiNode;

        ShaderDesc UIProgram{};

        std::shared_ptr<RenderBackEndI> renderBackEnd;

    private:
        void findAllLights(std::vector<DLight> &dLights, std::vector<PLight> &pLights);

        std::shared_ptr<FrameBuffer> screenBuffer;
        std::shared_ptr<AttachmentTexture> screenTexture;
        std::shared_ptr<AttachmentTexture> screenDepthTexture;

        std::unordered_map<UUID, ShaderDesc, UUID::hash> shader_storage;

        unsigned int width;
        unsigned int height;
    };
}
