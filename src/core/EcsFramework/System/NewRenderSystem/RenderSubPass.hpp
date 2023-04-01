#include "Render/RenderBackEndI.h"
#include "Render/PipeLine.hpp"
#include "Render/AttachmentTexture.h"

#include "EcsFramework/Component/MeshComponent.hpp"

#include <glm/glm/ext.hpp>
#include <glm/glm/gtx/euler_angles.hpp>
#include <string>

namespace SPW {
    struct AttachmentPack {
        public:
            std::shared_ptr<SPW::AttachmentTexture> texture = nullptr;
            std::shared_ptr<SPW::AttachmentTextureArray> textureArray = nullptr;
            std::shared_ptr<SPW::AttachmentTextureCube> textureCube = nullptr;
            std::shared_ptr<SPW::AttachmentTextureCubeArray> textureCubeArray = nullptr;
    };
    class SPWModelSubPass {
    public:
        SPWModelSubPass(const ModelSubPassConfig &config) {
            // precheck
            // can not have output and screen output at the same time
            assert(!(config.screenOutputType != UnknownScreenAttachmentType && config.outputType != UnknownAttachmentType));
            this->config = config;
        };

        void init(const std::shared_ptr<RenderBackEndI> &backend, unsigned int width, unsigned int height) {
            unsigned int frameWidth = config.frameWidth == 0 ? width : config.frameWidth;
            unsigned int frameHeight = config.frameHeight == 0 ? height : config.frameHeight;

            if (hasInternalFramebuffer()) {
                if (attchmentFrameBuffers.size() > 0 && (config.frameWidth != 0 && config.frameHeight != 0)) {
                    // if the framebuffer is already created, and the frame size is specified, then use the old size
                    return;
                }
                attchmentFrameBuffers.clear();
                attchmentTextures.clear();
                if (config.repeatType == NoRepeat) {
                    attchmentFrameBuffers.push_back(backend->createFrameBuffer());
                    attchmentFrameBuffers[0]->genFrameBuffer();
                    attchmentFrameBuffers[0]->bind();
                    unsigned int index = 0;
                    for (auto format : config.attachments) {
                        AttachmentPack texture;
                        if (config.outputType == ColorType) {
                            texture.texture = backend->createAttachmentTexture();
                            texture.texture->genTexture(frameWidth, frameHeight, format);
                            texture.texture->attach(index);
                        } else if (config.outputType == CubeMapType) {
                            texture.textureCube = backend->createAttachmentTextureCube();
                            texture.textureCube->genTexture(frameWidth, frameHeight, format);
                            // attch before draw
                            // texture.textureCube->attach(index, 0);
                        }
                        if (format != Depth) {
                            attchmentFrameBuffers[0]->AttachColorRenderBuffer(frameWidth,frameHeight,index++);
                        } else {
                            attchmentFrameBuffers[0]->AttachDepthRenderBuffer(frameHeight,frameHeight);
                        }
                        attchmentTextures.push_back(texture);
                    }
                    attchmentFrameBuffers[0]->CheckFramebufferStatus();
                    attchmentFrameBuffers[0]->unbind();
                } else {
                    unsigned int index = 0;
                    // has repeat type, create frame buffer array
                    for (unsigned int i = 0; i < config.repeatCount; i ++) {
                        attchmentFrameBuffers.push_back(backend->createFrameBuffer());
                        attchmentFrameBuffers[i]->genFrameBuffer();
                        attchmentFrameBuffers[i]->bind();

                        for (auto format : config.attachments) {
                            if (format != Depth) {
                                attchmentFrameBuffers[i]->AttachColorRenderBuffer(frameWidth,frameHeight,index++);
                            } else {
                                attchmentFrameBuffers[i]->AttachDepthRenderBuffer(frameHeight,frameHeight);
                            }
                        }
                    }
                    for (auto format : config.attachments) {
                        AttachmentPack texture;
                        unsigned int index = 0;
                        if (config.outputType == ColorType) {
                            texture.textureArray = backend->createAttachmentTextureArray();
                            texture.textureArray->genTexture(frameWidth, frameHeight, config.repeatCount, format);
                            for (unsigned int i = 0; i < config.repeatCount; i ++) {
                                attchmentFrameBuffers[i]->bind();
                                texture.textureArray->attach(index, i);
                                if (format != Depth) {
                                    index++;
                                }
                                attchmentFrameBuffers[i]->CheckFramebufferStatus();
                                attchmentFrameBuffers[i]->unbind();
                            }
                        } else if (config.outputType == CubeMapType) {
                            texture.textureCubeArray = backend->createAttachmentTextureCubeArray();
                            texture.textureCubeArray->genTexture(frameWidth, frameHeight, config.repeatCount, format);
                            // attch before draw;
                        }
                        attchmentTextures.push_back(texture);
                    }
                }
            }
        }

        void setViewPort(const std::shared_ptr<RenderBackEndI> &backend, unsigned int width, unsigned int height) {
            unsigned int frameWidth = config.frameWidth == 0 ? width : config.frameWidth;
            unsigned int frameHeight = config.frameHeight == 0 ? height : config.frameHeight;
            if (hasInternalFramebuffer()) {
                backend->SetViewport(0, 0, frameWidth, frameHeight);
            } else {
                backend->SetViewport(0, 0, width, height);
            }
            
            backend->DepthTest(config.depthTest);
            switch (config.depthCompType) {
            case EQUAL_Type: 
                backend->DepthFunc(DepthComp::EQUAL);
                break;
            case LEQUAL_Type:
                backend->DepthFunc(DepthComp::LEQUAL);
                break;
            case LESS_Type:
                backend->DepthFunc(DepthComp::LESS);
                break;
            }
        }

        void bindMaterial(
            const std::shared_ptr<Shader> &shader, 
            const std::shared_ptr<RenderBackEndI> &backend,
            const std::shared_ptr<Material> &material,
            unsigned int slot) 
        {
            for (auto &[type, name] : config.mat_inputs) {
                switch (type) {
                case AlbedoType: {
                    auto path = material->TextureMap[TextureType::Albedo];
                    shader->setInt(name, slot);
                    backend->BindImageTex(path, slot);
                    slot++;
                    break;
                }
                case NormalType: {
                    auto path = material->TextureMap[TextureType::Normal];
                    shader->setInt(name, slot);
                    backend->BindImageTex(path, slot);
                    slot++;
                    break;
                }
                case MetallicType: {
                    auto path = material->TextureMap[TextureType::Metalness];
                    shader->setInt(name, slot);
                    backend->BindImageTex(path, slot);
                    slot++;
                    break;
                }
                case RoughnessType: {
                    auto path = material->TextureMap[TextureType::Roughness];
                    shader->setInt(name, slot);
                    backend->BindImageTex(path, slot);
                    slot++;
                    break;
                }
                case AOType: {
                    auto path = material->TextureMap[TextureType::AmbientOcclusion];
                    shader->setInt(name, slot);
                    backend->BindImageTex(path, slot);
                    slot++;
                    break;
                }
                // TODO, temperary from these properties
                case AlbedoConstantType: {
                    shader->setVec3(name, material->m_Properties.ambientColor);
                    break;
                }
                case MetallicConstantType: {
                    shader->setFloat(name, material->m_Properties.metallicProperties.metallicFactor);
                    break;
                }
                case RoughnessConstantType: {
                    shader->setFloat(name, material->m_Properties.metallicProperties.roughnessFactor);
                    break;
                }
                case AOConstantType: {
                    shader->setFloat(name, material->m_Properties.metallicProperties.anisotropyFactor);
                    break;
                }
                case DiffuseType: {
                    shader->setVec3(name, material->m_Properties.diffuseColor);
                    break;
                }
                case SpecularType: {
                    shader->setVec3(name, material->m_Properties.specularColor);
                    break;
                }
                case AmbientType: {
                    shader->setVec3(name, material->m_Properties.ambientColor);
                    break;
                }
                case EmissiveType: {
                    shader->setVec3(name, material->m_Properties.emissiveColor);
                    break;
                }
                case EmissiveIntensityType: {
                    shader->setFloat(name, material->m_Properties.emissiveIntensity);
                    break;
                }
                case SpecularPowerType: {
                    shader->setFloat(name, material->m_Properties.shininessStrength);
                    break;
                }
                case CubeTextureType: {
                    if (material->cubeMapTextures.size() == 6) {
                        backend->BindCubeMap(material->cubeMapTextures, slot);
                        shader->setInt(name, slot);
                        slot ++;
                    }
                    break;
                }
                case TransparentType: {
                    shader->setVec3(name, material->m_Properties.transportColor);
                    break;
                }
                case RefractionType: {
                    shader->setVec3(name, material->m_Properties.refectiveColor);
                    break;
                }
                case TransparentIntensityType: {
                    shader->setFloat(name, material->m_Properties.transparentFactor);
                    break;
                }
                case RefractionIntensityType: {
                    shader->setFloat(name, material->m_Properties.refracti);
                    break;
                }
                break;
                }
            }
        }

        void bindMVP(const std::shared_ptr<Shader> &shader, 
            const glm::mat4 &model, const glm::mat4 &view, 
            const glm::mat4 &proj) {
            for (auto &[type, name] : config.transform_inputs) {
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
            float width, float height, const glm::vec4 &camPos) 
        {
            for (auto &[type, name] : config.context_inputs) {
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
                case RandomNumber:
                    shader->setFloat(name, float(rand()) / float(RAND_MAX));
                        break;
                }
            }
        }

        unsigned int bindPreviousAttachments(
            const std::shared_ptr<Shader> &shader, 
            const SPWModelSubPass &lastSubPass) 
        {
            unsigned int slot = 0;
            // precheck
            if (lastSubPass.config.screenOutputType == PresentType) {
                // last subpass is present subpass, nothing to bind
                assert(false);
            }

            // if previous subpass has no attachment output, nothing to bind
            // in this case, check if current subpass has input attachment
            // if current subpass has input attachment, assert
            if (lastSubPass.attchmentTextures.empty()) {
                assert(config.previous_attachment_inputs.empty());
                return slot;
            }

            // bind previous subpass attachment output
            for (auto &[idx, name] : config.previous_attachment_inputs) {
                assert(idx < lastSubPass.attchmentTextures.size());
                auto &texture = lastSubPass.attchmentTextures[idx];
                shader->setInt(name, slot);
                if (texture.texture) {
                    slot = texture.texture->bind(slot);
                } else if (texture.textureArray) {
                    slot = texture.textureArray->bind(slot);
                } else if (texture.textureCube) {
                    slot = texture.textureCube->bind(slot);
                } else if (texture.textureCubeArray) {
                    slot = texture.textureCubeArray->bind(slot);
                }
            }
            
            return slot;
        } 

        // bind lights
        void bindDlights(const std::shared_ptr<Shader> &shader, const glm::vec3 &cam_center, const std::vector<DLight> &dLights) {
            if (config.light_inputs.find(LightType::DirectionalLight) != config.light_inputs.end()) {
                for (unsigned int i = 0; i < dLights.size(); i ++) {
                    std::string name = config.light_inputs[LightType::DirectionalLight] + "["+std::to_string(i)+"]";
                    shader->setVec3(name+".direction", dLights[i].direction);
                    shader->setVec3(name+".ambient", dLights[i].ambient);
                    shader->setVec3(name+".diffuse", dLights[i].diffuse);
                    shader->setVec3(name+".specular", dLights[i].specular);
                }
            }

            if (config.light_inputs.find(LightType::DLightTransform) != config.light_inputs.end()) {
                for (unsigned int i = 0; i < dLights.size(); i ++) {
                    std::string name = config.light_inputs[LightType::DLightTransform] + "["+std::to_string(i)+"]";
                    bindDlightMat(shader, cam_center, name, dLights[i]);
                }
            }

            if (config.light_inputs.find(LightType::DLightArraySize) != config.light_inputs.end()) {
                std::string name = config.light_inputs[LightType::DLightArraySize];
                shader->setInt(name, dLights.size());
            }
        }

        void bindDlight(const std::shared_ptr<Shader> &shader, const glm::vec3 &cam_center, const DLight& light) {
            if (config.light_inputs.find(LightType::DirectionalLight) != config.light_inputs.end()) {
                std::string name = config.light_inputs[LightType::DirectionalLight];
                shader->setVec3(name+".direction", light.direction);
                shader->setVec3(name+".ambient", light.ambient);
                shader->setVec3(name+".diffuse", light.diffuse);
                shader->setVec3(name+".specular", light.specular);
            }

            if (config.light_inputs.find(LightType::DLightTransform) != config.light_inputs.end()) {
                std::string name = config.light_inputs[LightType::DLightTransform];
                bindDlightMat(shader, cam_center, name, light);
            }
        }

        void bindDlightMat(const std::shared_ptr<Shader> &shader, const glm::vec3 &cam_center, const std::string &name, const DLight& light) {
            glm::vec3 lightPos = cam_center - light.direction*5.0f;

            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;

            float near_plane = 1.0f, far_plane = 10.5f;
            lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
            lightView = glm::lookAt(lightPos, cam_center, glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;

            shader->setMat4(name, lightSpaceMatrix);
        }

        void bindPlight(const std::shared_ptr<Shader> &shader, const PLight& light) {
            if (config.light_inputs.find(LightType::PointLight) != config.light_inputs.end()) {
                std::string name = config.light_inputs[LightType::PointLight];
                shader->setVec3(name+".position", light.position);
                shader->setFloat(name+".constant", light.constant);
                shader->setFloat(name+".linear", light.linear);
                shader->setFloat(name+".quadratic", light.quadratic);
                shader->setVec3(name+".ambient", light.ambient);
                shader->setVec3(name+".diffuse", light.diffuse);
                shader->setVec3(name+".specular", light.specular);
            }
        }

        void bindPlights(const std::shared_ptr<Shader> &shader, const std::vector<PLight>& pLights) {
            if (config.light_inputs.find(LightType::PointLight) != config.light_inputs.end()) {
                for (unsigned int i = 0; i < pLights.size(); i ++) {
                    std::string name = config.light_inputs[LightType::PointLight] + "["+std::to_string(i)+"]";
                    shader->setVec3(name+".position", pLights[i].position);
                    shader->setFloat(name+".constant", pLights[i].constant);
                    shader->setFloat(name+".linear", pLights[i].linear);
                    shader->setFloat(name+".quadratic", pLights[i].quadratic);
                    shader->setVec3(name+".ambient", pLights[i].ambient);
                    shader->setVec3(name+".diffuse", pLights[i].diffuse);
                    shader->setVec3(name+".specular", pLights[i].specular);
                }
            }

            if (config.light_inputs.find(LightType::PLightArraySize) != config.light_inputs.end()) {
                std::string name = config.light_inputs[LightType::PLightArraySize];
                shader->setInt(name, pLights.size());
            }
        }

        bool hasInternalFramebuffer() {
            return config.screenOutputType == UnknownScreenAttachmentType && config.outputType != UnknownAttachmentType && !config.attachments.empty();
        }

        ModelSubPassConfig config;
        // store output frame buffer
        std::vector<std::shared_ptr<FrameBuffer>> attchmentFrameBuffers;
        // store output texture
        std::vector<AttachmentPack> attchmentTextures;
    };

    class SPWImageSubPass {
    public:
        SPWImageSubPass(const ImageSubPassConfig &config) {
            this->config = config;
        };

        void init(const std::shared_ptr<RenderBackEndI> &backend, unsigned int width, unsigned int height) {
            unsigned int frameWidth = config.frameWidth == 0 ? width : config.frameWidth;
            unsigned int frameHeight = config.frameHeight == 0 ? height : config.frameHeight;

            if (hasInternalFramebuffer()) {
                attchmentTextures.clear();
                internalFrameBuffer = backend->createFrameBuffer();
                internalFrameBuffer->genFrameBuffer();
                internalFrameBuffer->bind();
                unsigned int index = 0;
                for (auto format : config.attachments) {
                    std::shared_ptr<AttachmentTexture> texture = backend->createAttachmentTexture();
                    texture->genTexture(frameWidth, frameHeight, format);
                    texture->attach(index);
                    if (format != Depth) {
                        internalFrameBuffer->AttachColorRenderBuffer(frameWidth,frameHeight,index++);
                    } else {
                        internalFrameBuffer->AttachDepthRenderBuffer(frameHeight,frameHeight);
                    }
                    attchmentTextures.push_back(texture);
                }
                internalFrameBuffer->CheckFramebufferStatus();
                internalFrameBuffer->unbind();
            }
        }

        void setViewPort(const std::shared_ptr<RenderBackEndI> &backend, unsigned int width, unsigned int height) {
            unsigned int frameWidth = config.frameWidth == 0 ? width : config.frameWidth;
            unsigned int frameHeight = config.frameHeight == 0 ? height : config.frameHeight;
            if (hasInternalFramebuffer()) {
                backend->SetViewport(0, 0, frameWidth, frameHeight);
            } else {
                backend->SetViewport(0, 0, width, height);
            }
        }

        unsigned int bindPreviousModelAttachments(
            const std::shared_ptr<Shader> &shader, 
            const SPWModelSubPass &lastSubPass,
            unsigned int slot) 
        {
            // precheck
            if (lastSubPass.config.screenOutputType == PresentType) {
                // last subpass is present subpass, nothing to bind
                assert(false);
            }

            // if previous subpass has no attachment output, nothing to bind
            // in this case, check if current subpass has input attachment
            // if current subpass has input attachment, assert
            if (lastSubPass.attchmentTextures.empty()) {
                assert(config.previous_attachment_inputs.empty());
                return slot;
            }

            // bind previous subpass attachment output
            for (auto &[idx, name] : config.previous_attachment_inputs) {
                assert(idx < lastSubPass.attchmentTextures.size());
                auto &texture = lastSubPass.attchmentTextures[idx];
                shader->setInt(name, slot);
                if (texture.texture) {
                    slot = texture.texture->bind(slot);
                } else if (texture.textureArray) {
                    slot = texture.textureArray->bind(slot);
                } else if (texture.textureCube) {
                    slot = texture.textureCube->bind(slot);
                } else if (texture.textureCubeArray) {
                    slot = texture.textureCubeArray->bind(slot);
                }
            }

            return slot;
        } 

        unsigned int bindPreviousImageAttachments (
            const std::shared_ptr<Shader> &shader, 
            const SPWImageSubPass &lastSubPass, 
            unsigned int slot)
        {
            // precheck
            if (lastSubPass.config.screenOutputType == PresentType) {
                // last subpass is present subpass, nothing to bind
                assert(false);
            }

            // if previous subpass has no attachment output, nothing to bind
            // in this case, check if current subpass has input attachment
            // if current subpass has input attachment, assert
            if (lastSubPass.attchmentTextures.empty()) {
                assert(config.previous_attachment_inputs.empty());
                return slot;
            }

            // bind previous subpass attachment output
            for (auto &[idx, name] : config.previous_attachment_inputs) {
                assert(idx < lastSubPass.attchmentTextures.size());
                shader->setInt(name, slot);
                slot = lastSubPass.attchmentTextures[idx]->bind(slot);
            }

            return slot;
        }

        unsigned int bindScreenAttachments(
            const std::shared_ptr<Shader> &shader, 
            const std::shared_ptr<AttachmentTexture> &screenTexture, 
            const std::shared_ptr<AttachmentTexture> &screenDepthTexture,
            unsigned int slot) 
        {
            // bind previous subpass screen output
            for (auto &[type, name] : config.screen_attachment_inputs) {
                shader->setInt(name, slot);
                if (type == ScreenColorType) {
                    slot = screenTexture->bind(slot);
                } else if (type == ScreenDepthType) {
                    slot = screenDepthTexture->bind(slot);
                }
            }
            return slot;
        }

        void bindContext(const std::shared_ptr<Shader> &shader, 
            float width, float height, const glm::vec4 &camPos) 
        {
            for (auto &[type, name] : config.context_inputs) {
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
                case RandomNumber:
                    shader->setFloat(name, float(rand()) / float(RAND_MAX));
                        break;
                }
            }
        }

        // bind lights
        void bindDlights(const std::shared_ptr<Shader> &shader, const glm::vec3 &cam_center, const std::vector<DLight> &dLights) {
            if (config.light_inputs.find(LightType::DirectionalLight) != config.light_inputs.end()) {
                for (unsigned int i = 0; i < dLights.size(); i ++) {
                    std::string name = config.light_inputs[LightType::DirectionalLight] + "["+std::to_string(i)+"]";
                    shader->setVec3(name+".direction", dLights[i].direction);
                    shader->setVec3(name+".ambient", dLights[i].ambient);
                    shader->setVec3(name+".diffuse", dLights[i].diffuse);
                    shader->setVec3(name+".specular", dLights[i].specular);
                }
            }

            if (config.light_inputs.find(LightType::DLightTransform) != config.light_inputs.end()) {
                for (unsigned int i = 0; i < dLights.size(); i ++) {
                    std::string name = config.light_inputs[LightType::DLightTransform] + "["+std::to_string(i)+"]";
                    bindDlightMat(shader, cam_center, name, dLights[i]);
                }
            }

            if (config.light_inputs.find(LightType::DLightArraySize) != config.light_inputs.end()) {
                std::string name = config.light_inputs[LightType::DLightArraySize];
                shader->setInt(name, dLights.size());
            }
        }

        void bindDlightMat(const std::shared_ptr<Shader> &shader, const glm::vec3 &cam_center, const std::string &name, const DLight& light) {
            glm::vec3 lightPos = cam_center - light.direction*5.0f;

            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;

            float near_plane = 1.0f, far_plane = 10.5f;
            lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
            lightView = glm::lookAt(lightPos, cam_center, glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;

            shader->setMat4(name, lightSpaceMatrix);
        }

        void bindPlights(const std::shared_ptr<Shader> &shader, const std::vector<PLight>& pLights) {
            if (config.light_inputs.find(LightType::PointLight) != config.light_inputs.end()) {
                for (unsigned int i = 0; i < pLights.size(); i ++) {
                    std::string name = config.light_inputs[LightType::PointLight] + "["+std::to_string(i)+"]";
                    shader->setVec3(name+".position", pLights[i].position);
                    shader->setFloat(name+".constant", pLights[i].constant);
                    shader->setFloat(name+".linear", pLights[i].linear);
                    shader->setFloat(name+".quadratic", pLights[i].quadratic);
                    shader->setVec3(name+".ambient", pLights[i].ambient);
                    shader->setVec3(name+".diffuse", pLights[i].diffuse);
                    shader->setVec3(name+".specular", pLights[i].specular);
                }
            }
            
            if (config.light_inputs.find(LightType::PLightArraySize) != config.light_inputs.end()) {
                std::string name = config.light_inputs[LightType::PLightArraySize];
                shader->setInt(name, pLights.size());
            }
        }

        bool hasInternalFramebuffer() {
            return config.screenOutputType == UnknownScreenAttachmentType && config.outputType == ColorType && !config.attachments.empty();
        }

        ImageSubPassConfig config;
        std::shared_ptr<FrameBuffer> internalFrameBuffer;
        // store output texture
        std::vector<std::shared_ptr<AttachmentTexture>> attchmentTextures;
    };
}