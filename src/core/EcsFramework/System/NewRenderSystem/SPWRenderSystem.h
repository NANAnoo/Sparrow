#pragma once

#include "EcsFramework/Entity/Entity.hpp"
#include "Render/RenderBackEndI.h"
#include "Render/RenderGraph.hpp"

#include "EcsFramework/System/SystemI.h"
#include "EcsFramework/Component/CameraComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Component/BasicComponent/IDComponent.h"
#include "ApplicationFramework/WindowI/WindowEvent.h"

namespace SPW {

    class SPWRenderSystem : public SystemI, public WindowEventResponder {
    public:
        explicit SPWRenderSystem(std::shared_ptr<Scene> &scene, std::shared_ptr<RenderBackEndI> backEnd, int w, int h) :
            SystemI(scene), renderBackEnd(std::move(backEnd)),
            WindowEventResponder(std::dynamic_pointer_cast<EventResponderI>(scene)) {
                width = w;
                height = h;
            }
        void setupRenderBackEnd(const std::shared_ptr<RenderBackEndI> &backEnd) {
            renderBackEnd = backEnd;
        };

        void initial() final;
        void beforeUpdate() final;
        void onUpdate(TimeDuration dt) final {}
        void afterUpdate() final;
        void onStop() final;
        bool onFrameResize(int w, int h) override;

        std::shared_ptr<RenderGraph> createRenderGraph() {
            auto res = renderBackEnd->createRenderGraph();
            res->graph_id = graphs.size();
            graphs.push_back(res);
            return res;
        }

        void addShaderDesciptor(const ShaderDesc& desc) {
            shader_storage.insert({desc.uuid, desc});
        }
        // events
        const char *getName() override {return "SPW_RENDER_SYSTEM";}
    private:
        void findAllLights(std::vector<DLight> &dLights, std::vector<PLight> &pLights);

        std::shared_ptr<FrameBuffer> screenBuffer;
        std::shared_ptr<AttachmentTexture> screenTexture;
        std::shared_ptr<AttachmentTexture> screenDepthTexture;

        std::vector<std::shared_ptr<RenderGraph>> graphs;
        
        std::shared_ptr<RenderBackEndI> renderBackEnd;

        std::unordered_map<UUID, ShaderDesc, UUID::hash> shader_storage;

        unsigned int width;
        unsigned int height;
    };
}