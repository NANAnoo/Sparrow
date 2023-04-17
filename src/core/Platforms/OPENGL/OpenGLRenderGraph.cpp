#include "OpenGLRenderGraph.h"

#include "EcsFramework/Component/MeshComponent.hpp"
#include "EcsFramework/Component/TransformComponent.hpp"
#include "EcsFramework/Entity/Entity.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "IO/FileSystem.h"


namespace SPW {
    static const glm::mat4 getCubeViewOnFace(const glm::vec3 &pos, int face) {
        // calculate view matrix for face
        glm::mat4 view;
        switch (face) {
            case 0:
                view = glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0));
                break;
            case 1:
                view = glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0));
                break;
            case 2:
                view = glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
                break;
            case 3:
                view = glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1));
                break;
            case 4:
                view = glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
                break;
            case 5:
                view = glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0));
                break;
        }
        return view;
    }

    static const glm::mat4 getCubeProjection() {
        return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
    }

    void bindMaterial(
            const std::shared_ptr<Shader> &shader,
             const std::unordered_map<MaterialType, std::string> &mat_inputs,
            const std::shared_ptr<RenderBackEndI> &backend,
            const MaterialData& material,
			const std::unordered_map<std::string, std::string>& textures,
            unsigned int slot) 
    {


        std::string albedo_path = FileRoots::k_Root;
        auto albedo_it = material.m_TextureIDMap.find(TextureMapType::Albedo);
        if (albedo_it != material.m_TextureIDMap.end()) {
            albedo_path += textures.at(albedo_it->second);
        }

        std::string normal_path = FileRoots::k_Root;
        auto normal_it = material.m_TextureIDMap.find(TextureMapType::Normal);
        if (normal_it != material.m_TextureIDMap.end()) {
            normal_path += textures.at(normal_it->second);
        }

        std::string metalness_path = FileRoots::k_Root;
        auto metalness_it = material.m_TextureIDMap.find(TextureMapType::Metalness);
        if (metalness_it != material.m_TextureIDMap.end()) {
            metalness_path += textures.at(metalness_it->second);
        }

        std::string roughness_path = FileRoots::k_Root;
        auto roughness_it = material.m_TextureIDMap.find(TextureMapType::Roughness);
        if (roughness_it != material.m_TextureIDMap.end()) {
            roughness_path += textures.at(roughness_it->second);
        }

        std::string ao_path = FileRoots::k_Root;
        auto ao_it = material.m_TextureIDMap.find(TextureMapType::AmbientOcclusion);
        if (ao_it != material.m_TextureIDMap.end()) {
            ao_path += textures.at(ao_it->second);
        }
        for (auto &[type, name] : mat_inputs) 
        {
            switch (type)
        	{
            case AlbedoType: 
            {
                // auto path = material->TextureMap[TextureType::Albedo];
                shader->setInt(name, slot);
                backend->BindImageTex(albedo_path, slot);
                slot++;
                break;
            }
            case NormalType: 
            {
                // auto path = material->TextureMap[TextureType::Normal];
                shader->setInt(name, slot);
                backend->BindImageTex(normal_path, slot);
                slot++;
                break;
            }
            case MetallicType: 
            {
                // auto path = material->TextureMap[TextureType::Metalness];
                shader->setInt(name, slot);
                backend->BindImageTex(metalness_path, slot);
                slot++;
                break;
            }
            case RoughnessType: 
            {
                // auto path = material->TextureMap[TextureType::Roughness];
                shader->setInt(name, slot);
                backend->BindImageTex(roughness_path, slot);
                slot++;
                break;
            }
            case AOType: 
            {
                // auto path = material->TextureMap[TextureType::AmbientOcclusion];
                shader->setInt(name, slot);
                backend->BindImageTex(ao_path, slot);
                slot++;
                break;
            }
            // TODO, temperary from these properties
            case AlbedoConstantType: 
            {
                shader->setVec3(name, material.m_Properties.ambient);
                break;
            }
            case MetallicConstantType: {
                shader->setFloat(name, material.m_Properties.metallicConstant);
                break;
            }
            case RoughnessConstantType: {
                shader->setFloat(name, material.m_Properties.roughnessConstant);
                break;
            }
            case AOConstantType: {
                shader->setFloat(name, material.m_Properties.AOConstant);
                break;
            }
            case DiffuseType: {
                shader->setVec3(name, material.m_Properties.diffuse);
                break;
            }
            case SpecularType: {
                shader->setVec3(name, material.m_Properties.specular);
                break;
            }
            case AmbientType: {
                shader->setVec3(name, material.m_Properties.ambient);
                break;
            }
            case EmissiveType: {
                shader->setVec3(name, material.m_Properties.emissive);
                break;
            }
            case EmissiveIntensityType: {
                shader->setFloat(name, material.m_Properties.emissiveIntensity);
                break;
            }
            case SpecularPowerType: {
                shader->setFloat(name, material.m_Properties.specularPower);
                break;
            }
            case CubeTextureType: 
            {
                std::vector<std::string> cubeMapPaths =
                {
                    k_Engine + "texture/skybox/top.jpg",
                    k_Engine + "texture/skybox/top.jpg",
                    k_Engine + "texture/skybox/top.jpg",

                    k_Engine + "texture/skybox/top.jpg",
                    k_Engine + "texture/skybox/top.jpg",
                    k_Engine + "texture/skybox/top.jpg",
                };

                // if (material->cubeMapTextures.size() == 6) 
                {
                    backend->BindCubeMap(cubeMapPaths, slot);
                    shader->setInt(name, slot);
                    slot ++;
                }
                break;
            }
            case TransparentType: {
                shader->setVec3(name, material.m_Properties.transparent);
                break;
            }
            case RefractionType: {
                shader->setVec3(name, material.m_Properties.refection);
                break;
            }
            case TransparentIntensityType: {
                shader->setFloat(name, material.m_Properties.transparentIntensity);
                break;
            }
            case RefractionIntensityType: {
                shader->setFloat(name, material.m_Properties.refractionIntensity);
                break;
            }
            break;
            }
        }
    }

    void bindMVP(const std::shared_ptr<Shader> &shader, 
        const std::unordered_map<TransformType, std::string> &transform_inputs,
        const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj) 
    {
        for (auto &[type, name] : transform_inputs) {
            switch (type) {
                case V: {
                    shader->setMat4(name, view);
                    break;
                }
                case P: {
                    shader->setMat4(name, proj);
                    break;
                }
                case VP: {
                    shader->setMat4(name, proj * view);
                    break;
                }
                case M: {
                    shader->setMat4(name, model);
                    break;
                }
                case MVP: {
                    shader->setMat4(name, proj * view * model);
                    break;
                }
                case MV: {
                    shader->setMat4(name, view * model);
                    break;
                }
            }
        }
    }

    void bindContext(const std::shared_ptr<Shader> &shader, 
        const std::unordered_map<ContextType, std::string> &context_inputs,
        float width, float height, const glm::vec3 &camPos) 
    {
        for (auto &[type, name] : context_inputs) {
            switch (type) {
            case FrameWidth: {
                shader->setFloat(name, float(width));
                break;
            }
            case FrameHeight: {
                shader->setFloat(name, float(height));
                break;
            }
            case CameraPosition: {
                shader->setVec3(name, camPos);
                break;
            }
            case RandomNumber: {
                shader->setFloat(name, float(rand()) / float(RAND_MAX));
                break;
            }
            default:
                break;
            }
        }
    }

    // bind lights
    void bindDlightMat(const std::shared_ptr<Shader> &shader, const glm::vec3 &cam_center, const std::string &name, const DLight& light) {
        glm::vec3 lightPos = cam_center - light.direction*5.0f;

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;

        float near_plane = 1.0f, far_plane = 10.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, cam_center, glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        shader->setMat4(name, lightSpaceMatrix);
    }
    
    void bindDlights(const std::shared_ptr<Shader> &shader, const std::unordered_map<LightType, std::string> &light_inputs, 
        const glm::vec3 &cam_center, const std::vector<DLight> &dLights) {
        if (light_inputs.find(LightType::DirectionalLight) != light_inputs.end()) {
            for (unsigned int i = 0; i < dLights.size(); i ++) {
                std::string name = light_inputs.at(LightType::DirectionalLight) + "["+std::to_string(i)+"]";
                shader->setVec3(name+".direction", dLights[i].direction);
                shader->setVec3(name+".ambient", dLights[i].ambient);
                shader->setVec3(name+".diffuse", dLights[i].diffuse);
                shader->setVec3(name+".specular", dLights[i].specular);
            }
        }

        if (light_inputs.find(LightType::DLightTransform) != light_inputs.end()) {
            for (unsigned int i = 0; i < dLights.size(); i ++) {
                std::string name = light_inputs.at(LightType::DLightTransform) + "["+std::to_string(i)+"]";
                bindDlightMat(shader, cam_center, name, dLights[i]);
            }
        }

        if (light_inputs.find(LightType::DLightArraySize) != light_inputs.end()) {
            std::string name = light_inputs.at(LightType::DLightArraySize);
            shader->setInt(name, dLights.size());
        }
    }

    void bindDlight(const std::shared_ptr<Shader> &shader, const std::unordered_map<LightType, std::string> &light_inputs, 
        const glm::vec3 &cam_center, const DLight& light) {
        if (light_inputs.find(LightType::DirectionalLight) != light_inputs.end()) {
            std::string name = light_inputs.at(LightType::DirectionalLight);
            shader->setVec3(name+".direction", light.direction);
            shader->setVec3(name+".ambient", light.ambient);
            shader->setVec3(name+".diffuse", light.diffuse);
            shader->setVec3(name+".specular", light.specular);
        }

        if (light_inputs.find(LightType::DLightTransform) != light_inputs.end()) {
            std::string name = light_inputs.at(LightType::DLightTransform);
            bindDlightMat(shader, cam_center, name, light);
        }
    }

    void bindPlight(const std::shared_ptr<Shader> &shader, const std::unordered_map<LightType, std::string> &light_inputs, const PLight& light) {
        if (light_inputs.find(LightType::PointLight) != light_inputs.end()) {
            std::string name = light_inputs.at(LightType::PointLight);
            shader->setVec3(name+".position", light.position);
            shader->setFloat(name+".constant", light.constant);
            shader->setFloat(name+".linear", light.linear);
            shader->setFloat(name+".quadratic", light.quadratic);
            shader->setVec3(name+".ambient", light.ambient);
            shader->setVec3(name+".diffuse", light.diffuse);
            shader->setVec3(name+".specular", light.specular);
        }
    }

    void bindPlights(const std::shared_ptr<Shader> &shader, const std::unordered_map<LightType, std::string> &light_inputs, const std::vector<PLight>& pLights) {
        if (light_inputs.find(LightType::PointLight) != light_inputs.end()) {
            for (unsigned int i = 0; i < pLights.size(); i ++) {
                std::string name = light_inputs.at(LightType::PointLight) + "["+std::to_string(i)+"]";
                shader->setVec3(name+".position", pLights[i].position);
                shader->setFloat(name+".constant", pLights[i].constant);
                shader->setFloat(name+".linear", pLights[i].linear);
                shader->setFloat(name+".quadratic", pLights[i].quadratic);
                shader->setVec3(name+".ambient", pLights[i].ambient);
                shader->setVec3(name+".diffuse", pLights[i].diffuse);
                shader->setVec3(name+".specular", pLights[i].specular);
            }
        }

        if (light_inputs.find(LightType::PLightArraySize) != light_inputs.end()) {
            std::string name = light_inputs.at(LightType::PLightArraySize);
            shader->setInt(name, pLights.size());
        }
    }

    inline void clearHelper(const std::shared_ptr<RenderPassNodeI> &node, const std::shared_ptr<RenderBackEndI> &backend) {
        if ((node->clearType & ClearType::ClearColor) > 0) {
            backend->ClearColor();
        } 
        if ((node->clearType & ClearType::ClearDepth) > 0) {
            backend->ClearDepth();
        }
    }
    using ModelNodeMap = std::unordered_map<unsigned int, std::unordered_map<UUID, std::vector<Entity>, UUID::hash>>;
    void OpenGLRenderGraph::init() {
        // create all required resources
        for (auto &[idx, node] : all_nodes) {
            if (std::dynamic_pointer_cast<ModelRepeatPassNode>(node)) {
                createResourceForModelRepeatNode(std::dynamic_pointer_cast<ModelRepeatPassNode>(node));
            } else if (std::dynamic_pointer_cast<ModelPassNode>(node)) {
                createResourceForModelNode(std::dynamic_pointer_cast<ModelPassNode>(node));
            }
        }

        // fina the node index that no node depend on it, which is the output node
        std::unordered_set<unsigned int> all_node_idx;
        std::unordered_set<unsigned int> node_is_depended;
        std::unordered_set<unsigned int> outputs;
        for (auto &[idx, node] : all_nodes) {
            all_node_idx.insert(idx);
            for (auto &port : node->input_ports) {
                if (port.pass_id != SCREEN_PORT) 
                    node_is_depended.insert(port.pass_id);
            }
        }
        std::set_difference(
            all_node_idx.begin(), all_node_idx.end(), 
            node_is_depended.begin(), node_is_depended.end(),
            std::inserter(outputs, outputs.begin())
        );
        // should only have one output
        assert(outputs.size() == 1);
        output_node_id = *outputs.begin();
        auto &node = all_nodes.at(output_node_id);

        // out put node should output to screen or present
        assert(
            std::dynamic_pointer_cast<PresentNode>(node) != nullptr || 
            std::dynamic_pointer_cast<ModelToScreenNode>(node)!= nullptr || 
            std::dynamic_pointer_cast<ScreenPassNode>(node)!= nullptr
        );
    }

    void OpenGLRenderGraph::render(const RenderInput &input) {
        for (auto &[idx, node] : all_nodes) {
            node->ready = false;
        }
        ModelNodeMap model_map;
        for (auto &[id, models] : input.render_models) {
            for (auto &model : models) {
                // pass id -> shader id
                auto registered_shaders = model.component<MeshComponent>()->modelSubPassPrograms;
                if (registered_shaders.find(id) == registered_shaders.end()) {
                    continue;
                }
                model_map[id][registered_shaders[id]].push_back(model);
            }
        }
        renderOnNode(output_node_id, model_map, input);
    }

    // render on node
    void OpenGLRenderGraph::renderOnNode(unsigned int idx, const ModelNodeMap& map, const RenderInput &input) {
        if (all_nodes.find(idx) == all_nodes.end()) {
            return;
        }
        std::shared_ptr<RenderPassNodeI> node = all_nodes.at(idx);
        if (node->ready) {
            return;
        }
        // make sure all input nodes are ready
        for (auto &port : node->input_ports) {
            renderOnNode(port.pass_id, map, input);
        }
        input.backend->DepthTest(node->depthTest);
        switch (node->depthCompType) {
        case EQUAL_Type: 
            input.backend->DepthFunc(DepthComp::EQUAL);
            break;
        case LEQUAL_Type:
            input.backend->DepthFunc(DepthComp::LEQUAL);
            break;
        case LESS_Type:
            input.backend->DepthFunc(DepthComp::LESS);
            break;
        }
        // render on current node
        renderOnModelRepeatPassNode(std::dynamic_pointer_cast<ModelRepeatPassNode>(node), map, input);
        renderOnModelPassNode(std::dynamic_pointer_cast<ModelPassNode>(node), map, input);
        renderOnModelToScreenNode(std::dynamic_pointer_cast<ModelToScreenNode>(node), map, input);
        renderOnScreenPassNode(std::dynamic_pointer_cast<ScreenPassNode>(node), input);
        renderOnPresentNode(std::dynamic_pointer_cast<PresentNode>(node), input);
    }

    void OpenGLRenderGraph::drawWithModels(const RenderInput &input, const std::unordered_map<UUID, std::vector<Entity>, UUID::hash> &render_models, RepeatType type, unsigned int repeat_id, int face) {
        for (auto &[shader_id, models] : render_models) {
            auto shader_desc = input.shader_map.at(shader_id);
            auto shader = input.backend->getShader(shader_desc.shader);
            shader->Bind();
            bindContext(shader, shader_desc.context_inputs, input.screen_width, input.screen_height, input.camera_pos);
            // bind lights
            if (type == NoRepeat) {
                bindDlights(shader, shader_desc.light_inputs, input.camera_pos, input.dLights);
                bindPlights(shader, shader_desc.light_inputs, input.pLights);
            } else if (type == RepeatForDLights) {
                bindDlight(shader, shader_desc.light_inputs, input.camera_pos, input.dLights[repeat_id]);
            } else if (type == RepeatForPLights) {
                bindPlight(shader, shader_desc.light_inputs, input.pLights[repeat_id]);
            }
            // bind attachments
            unsigned int slot = bindAttachments(shader, shader_desc, input.screen_texture, input.screen_texture, 0);
            for (auto &model : models) {
                auto mesh_component = model.component<MeshComponent>();
                auto transform = model.component<TransformComponent>();
                glm::mat4 MM = glm::mat4(1.0f);
                MM = glm::translate(MM,transform->position);
                MM = MM * glm::eulerAngleYXZ(glm::radians(transform->rotation.y),
                                            glm::radians(transform->rotation.x),
                                            glm::radians(transform->rotation.z));
                MM = glm::scale(MM, transform->scale);
                if (face < 0) {
                    bindMVP(shader, shader_desc.transform_inputs, MM, input.view, input.projection);
                } else {
                    if (type == RepeatForPLights) {
                        bindMVP(shader, shader_desc.transform_inputs, MM,getCubeViewOnFace(input.pLights[repeat_id].position, face), getCubeProjection());
                    } else {
                        bindMVP(shader, shader_desc.transform_inputs, MM,getCubeViewOnFace(glm::vec3(0.f), face), getCubeProjection());
                    }
                }
                
                if (mesh_component->onDraw)
				{
                    RenderCommandsQueue<Shader> queue;
					mesh_component->onDraw(queue);
                    queue.executeWithAPI(shader);
                }

                // draw submeshes
                auto& meshes = ResourceManager::getInstance()->m_AssetDataMap[mesh_component->assetName].meshes;
                auto& materials = ResourceManager::getInstance()->m_AssetDataMap[mesh_component->assetName].materials;
                auto& textures = ResourceManager::getInstance()->m_AssetDataMap[mesh_component->assetName].textures;
                
                for (int idx = 0; idx < meshes.size() ; ++idx)
                {
                    bindMaterial(shader, shader_desc.mat_inputs, input.backend, materials[idx], textures, slot);
                    meshes[idx].PureDraw(input.backend);
                }
            }
        }
    }

    // render on model pass node
    void OpenGLRenderGraph::renderOnModelPassNode(const std::shared_ptr<ModelPassNode> &node, const ModelNodeMap& map, const RenderInput &input) {
        if (!node || node->ready) {
            return;
        }
        if (map.find(node->pass_id) == map.end()) {
            node->ready = true;
            return;
        }
        all_frame_buffers[node->frame_buffer_ref]->bind();
        input.backend->SetViewport(0, 0, node->width, node->height);
        if (node->output_type == ColorType) {
            clearHelper(node, input.backend);
            drawWithModels(input, map.at(node->pass_id), NoRepeat, 0, -1);
        } else {
            // draw on cube map
            for (unsigned int face = 0; face < 6; face ++) {
                // attach one face to the framebuffer
                unsigned int attachIndex = 0;
                for (int attachID = 0; attachID < node->attachment_formats.size(); attachID ++) {
                    auto &cube = all_attachment_cubes.at({node->pass_id, attachID});
                    auto format = node->attachment_formats[attachID];
                    if (format == Depth) {
                        cube->attach(0, face);
                    } else {
                        cube->attach(attachIndex ++, face);
                    }
                }
                // check status
                all_frame_buffers[node->frame_buffer_ref]->CheckFramebufferStatus();
                // clear before draw
                clearHelper(node, input.backend);
                drawWithModels(input, map.at(node->pass_id), NoRepeat, 0, face);
            }
        }
        all_frame_buffers[node->frame_buffer_ref]->unbind();
        node->ready = true;
    }

    // render on model repeat pass node
    void OpenGLRenderGraph::renderOnModelRepeatPassNode(const std::shared_ptr<ModelRepeatPassNode> &node, const ModelNodeMap& map, const RenderInput &input) {
        if (!node || node->ready) {
            return;
        }
        if (map.find(node->pass_id) == map.end()) {
            node->ready = true;
            return;
        }
        RepeatType type = node->repeat_type;
        int max_repeat_count = std::min(int(node->repeat_count), int(type == RepeatForPLights ? input.pLights.size() : input.dLights.size()));
        for (unsigned int repeat_id = 0; repeat_id < max_repeat_count; repeat_id ++) {
            auto fb_id = node->frame_buffer_refs[repeat_id];
            all_frame_buffers[fb_id]->bind();
            input.backend->SetViewport(0, 0, node->width, node->height);
            if (node->output_type == ColorType) {
                clearHelper(node, input.backend);
                drawWithModels(input, map.at(node->pass_id), type, repeat_id, -1);
            } else {
                // draw on cube map
                for (unsigned int face = 0; face < 6; face ++) {
                    // attach one face to the framebuffer
                    unsigned int attachIndex = 0;
                    for (int attachID = 0; attachID < node->attachment_formats.size(); attachID ++) {
                        auto &cubeArray = all_attachment_cube_arrays.at({node->pass_id, attachID});
                        auto format = node->attachment_formats[attachID];
                        if (format == Depth) {
                            cubeArray->attach(0, repeat_id, face);
                        } else {
                            cubeArray->attach(attachIndex ++, repeat_id, face);
                        }
                    }
                    // check status
                    all_frame_buffers[fb_id]->CheckFramebufferStatus();
                    // clear before draw
                    clearHelper(node, input.backend);
                    drawWithModels(input, map.at(node->pass_id), type, repeat_id, face);
                }
            }
            all_frame_buffers[fb_id]->unbind();
        }
        node->ready = true;
    }

    // render on model to screen pass node
    void OpenGLRenderGraph::renderOnModelToScreenNode(const std::shared_ptr<ModelToScreenNode> &node, const ModelNodeMap& map, const RenderInput &input) {
        if (!node || node->ready) {
            return;
        }
        if (map.find(node->pass_id) == map.end()) {
            node->ready = true;
            return;
        }
        input.screen_buffer->bind();
        input.backend->SetViewport(0, 0, input.screen_width, input.screen_height);
        drawWithModels(input, map.at(node->pass_id), NoRepeat, 0, -1);
        input.screen_buffer->unbind();
        node->ready = true;
    }

    // render on screen pass node
    void OpenGLRenderGraph::renderOnScreenPassNode(const std::shared_ptr<ScreenPassNode> &node, const RenderInput &input) {
        if (!node || node->ready) {
            return;
        }
        input.screen_buffer->bind();
        renderOnPresentNode(node, input);
        input.screen_buffer->unbind();
        node->ready = true;
    }

    // render on present node
    void OpenGLRenderGraph::renderOnPresentNode(const std::shared_ptr<PresentNode> &node, const RenderInput &input) {
        if (!node || node->ready) {
            return;
        }
        input.backend->SetViewport(0, 0, input.screen_width, input.screen_height);
        // draw screen quad
        auto shader = input.backend->getShader(node->shader.shader);
        shader->Bind();
        bindContext(shader, node->shader.context_inputs, input.screen_width, input.screen_height, input.camera_pos);
        bindAttachments(shader, node->shader, input.screen_texture, input.screen_texture, 0);
        
        bindPlights(shader, node->shader.light_inputs, input.pLights);
        bindDlights(shader, node->shader.light_inputs, input.camera_pos, input.dLights);
        input.backend->drawInQuad();
        node->ready = true;
    }

    // create resource for model node
    void OpenGLRenderGraph::createResourceForModelNode(const std::shared_ptr<ModelPassNode> &node) {
        if (!node || node->attachment_formats.size() == 0) {
            return;
        }
        // create resource for model
        node->frame_buffer_ref = all_frame_buffers.size();
        auto fb = std::make_shared<OpenGLFrameBuffer>();
        all_frame_buffers.push_back(fb);
        fb->genFrameBuffer();
        fb->bind();
        unsigned int index = 0;
        int resources_id = 0;
        for (auto &format : node->attachment_formats) {
            if (node->output_type == ColorType) {
                auto tex = std::make_shared<OpenGLAttachmentTexture>();
                tex->genTexture(node->width, node->height, format);
                tex->attach(index);
                if (format != ColorAttachmentFormat::Depth) {
                    fb->AttachColorRenderBuffer(node->width, node->height, index++);
                } else {
                    fb->AttachDepthRenderBuffer(node->width, node->height);
                }
                tex->unbind();
                all_attachments.insert({{node->pass_id, resources_id}, tex});
            } else {
                auto cube = std::make_shared<OpenGLAttachmentTextureCube>();
                cube->genTexture(node->width, node->height, format);
                cube->unbind();
                // attach while rendering
                all_attachment_cubes.insert({{node->pass_id, resources_id}, cube});
            }
            resources_id ++;
        }
        fb->CheckFramebufferStatus();
        fb->unbind();
    }

    // create resource for model repeat node
    void OpenGLRenderGraph::createResourceForModelRepeatNode(const std::shared_ptr<ModelRepeatPassNode> &node) {
        if (!node || node->attachment_formats.size() == 0) {
            return;
        }
        // create resource for model
        unsigned int fb_index = all_frame_buffers.size();
        for (unsigned int idx = 0; idx < node->repeat_count; idx ++) {
            node->frame_buffer_refs.push_back(fb_index + idx);
            auto fb = std::make_shared<OpenGLFrameBuffer>();
            all_frame_buffers.push_back(fb);
            fb->genFrameBuffer();
            fb->bind();
            unsigned int index = 0;
            for (auto &format : node->attachment_formats) {
                if (format != Depth) {
                    fb->AttachColorRenderBuffer(node->width,node->height,index++);
                } else {
                    fb->AttachDepthRenderBuffer(node->width,node->height);
                }
            }
            fb->unbind();
        }
        int resources_id = 0;
        for (auto format : node->attachment_formats) {
            unsigned int index = 0;
            if (node->output_type == ColorType) {
                auto textureArray = std::make_shared<OpenGLAttachmentTextureArray>();
                textureArray->genTexture(node->width, node->height, node->repeat_count, format);
                for (unsigned int i = 0; i < node->repeat_count; i ++) {
                    auto &fb = all_frame_buffers[node->frame_buffer_refs[i]];
                    fb->bind();
                    textureArray->attach(index, i);
                    if (format != Depth) {
                        index++;
                    }
                    fb->CheckFramebufferStatus();
                    fb->unbind();
                }
                all_attachment_arrays.insert({{node->pass_id, resources_id}, textureArray});
            } else if (node->output_type == CubeMapType) {
                auto cube_array = std::make_shared<OpenGLAttachmentTextureCubeArray>();
                cube_array->genTexture(node->width, node->height, node->repeat_count, format);
                // attch before draw;
                all_attachment_cube_arrays.insert({{node->pass_id, resources_id},cube_array});
            }
            resources_id ++;
        }
    }

    unsigned int OpenGLRenderGraph::bindAttachments(
        const std::shared_ptr<Shader> &shader, 
        const ShaderDesc &desc,
        const std::shared_ptr<AttachmentTexture> &screen_color, 
        const std::shared_ptr<AttachmentTexture> &screen_depth, 
        unsigned int slot) 
    {
        for (auto &[port, name] : desc.dependency_inputs) {
            shader->setInt(name, slot);
            if (port.screen_resource != UnknownScreenAttachmentType) {
                if (port.screen_resource == ScreenColorType) {
                    slot = screen_color->use(slot);
                }
            } else if (port.resource_id >= 0) {
                if (all_attachments.find(port) != all_attachments.end()) {
                    slot = all_attachments.at(port)->use(slot);
                } else if (all_attachment_arrays.find(port) != all_attachment_arrays.end()) {
                    slot = all_attachment_arrays.at(port)->use(slot);
                } else if (all_attachment_cubes.find(port) != all_attachment_cubes.end()) {
                    slot = all_attachment_cubes.at(port)->use(slot);
                } else if (all_attachment_cube_arrays.find(port) != all_attachment_cube_arrays.end()) {
                    slot = all_attachment_cube_arrays.at(port)->use(slot);
                }
            }
        }
        return slot;
    }
}
