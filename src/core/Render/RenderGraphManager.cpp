//
// Created by NANAnoo on 5/9/2023.
//
#include "RenderGraphManager.h"
#include "DefaultRenderPass.hpp"

unsigned int SPW::RenderGraphManager::getRenderGraph(const RenderGraphKey &type) {
    if (graphs.contains(type)) {
        return graphs.at(type)->graph_id;
    }
    std::printf("RenderGraph with type (%s) not exists", type.c_str());
    return UNKNOWN_ID;
}

unsigned int SPW::RenderGraphManager::getRenderNode(const RenderNodeKey &type) {
    if (nodes.contains(type)) {
        return nodes.at(type)->pass_id;
    }
    std::printf("RenderNode with type (%s) not exists", type.c_str());
    return UNKNOWN_ID;
}

SPW::AttachmentPort SPW::RenderGraphManager::getRenderNodeOutput(const RenderNodeOutputKey &type) {
    if (all_ports.contains(type)) {
        return all_ports.at(type);
    }
    std::printf("RenderOutput with type (%s) not exists", type.c_str());
    return {UNKNOWN_ID, 0};
}


SPW::ShaderDesc SPW::RenderGraphManager::getShaderDesc(const SPW::ShaderDescKey &type) {
    if (shaders.contains(type)) {
        return shaders.at(type);
    }
    std::printf("Shader (%s) not exists", type.c_str());
    return SPW::ShaderDesc({});
}

void
SPW::RenderGraphManager::createRenderGraph(const std::shared_ptr<RenderBackEndI> &backend, const RenderGraphKey &type) {
    if (type == kDefferShadingGraph) {
        auto defferShading = backend->createRenderGraph();
        defferShading->graph_id = (unsigned int)(graphs.size());

        auto p_shadowMap_node = defferShading->createRenderNode<SPW::ModelRepeatPassNode>(SPW::CubeMapType, SPW::RepeatForPLights, 10);
        p_shadowMap_node->width = 256;
        p_shadowMap_node->height = 256;
        p_shadowMap_node->clearType = SPW::ClearDepth;

        auto d_shadowMap_node = defferShading->createRenderNode<SPW::ModelRepeatPassNode>(SPW::ColorType, SPW::RepeatForDLights, 10);
        d_shadowMap_node->width = 4096;
        d_shadowMap_node->height = 4096;
        d_shadowMap_node->clearType = SPW::ClearDepth;

        auto p_shadowMap_output = p_shadowMap_node->addAttachment(SPW::Depth);
        auto d_shadowMap_output = d_shadowMap_node->addAttachment(SPW::Depth);

        auto defferNode = defferShading->createRenderNode<SPW::ModelPassNode>(SPW::ColorType);
        defferNode->clearType = SPW::ClearType::ClearAll;
        defferNode->depthTest = true;
        defferNode->depthCompType = SPW::DepthCompType::LESS_Type;
        auto gPosition = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGBA32);
        auto gNormal = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGB16);
        auto gAlbedo = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGBA32);
        auto gMetalRoughnessAO = defferNode->addAttachment(SPW::ColorAttachmentFormat::RGBA32);
        auto gDepth = defferNode->addAttachment(SPW::ColorAttachmentFormat::Depth);

        auto pbr_deffer_shading_desc = SPW::defferPBR(p_shadowMap_output, d_shadowMap_output, gPosition, gNormal, gAlbedo, gMetalRoughnessAO, gDepth, {
                "defferPBR",
                "./resources/shaders/pbr_defer_shading.vert",
                "./resources/shaders/pbr_defer_shading.frag"
        });

        auto GBufferShading = defferShading->createRenderNode<SPW::ImagePassNode>(pbr_deffer_shading_desc);
        GBufferShading->bindInputPort(gPosition);
        GBufferShading->bindInputPort(gNormal);
        GBufferShading->bindInputPort(gAlbedo);
        GBufferShading->bindInputPort(gMetalRoughnessAO);
        GBufferShading->bindInputPort(p_shadowMap_output);
        GBufferShading->bindInputPort(d_shadowMap_output);
        GBufferShading->bindInputPort(gDepth);
        GBufferShading->depthCompType = SPW::DepthCompType::LESS_Type;

        SPW::AttachmentPort shading_result = GBufferShading->addAttachment(SPW::RGBA32);

        auto SSR_desc = SPW::SSR(gAlbedo, gMetalRoughnessAO, gDepth, gNormal, gPosition, shading_result);
        auto SSR_node = defferShading->createRenderNode<SPW::ImagePassNode>(SSR_desc);
        SSR_node->clearType = SPW::ClearColor;
        SSR_node->depthTest = false;
        SSR_node->width = 1024;
        SSR_node->height = 1024;

        SSR_node->bindInputPort(gPosition);
        SSR_node->bindInputPort(gNormal);
        SSR_node->bindInputPort(gAlbedo);
        SSR_node->bindInputPort(gMetalRoughnessAO);
        SSR_node->bindInputPort(gDepth);
        SSR_node->bindInputPort(shading_result);
        auto reflect_port = SSR_node->addAttachment(SPW::RGBA32);

        auto SSR_blur_desc = SPW::SSR_blur(reflect_port,gDepth,shading_result);
        auto SSR_blur_node = defferShading->createRenderNode<SPW::ScreenPassNode>(SSR_blur_desc);
        SSR_blur_node->clearType = SPW::ClearAll;
        SSR_blur_node->depthTest = true;

        SSR_blur_node->bindInputPort(reflect_port);
        SSR_blur_node->bindInputPort(gDepth);
        SSR_blur_node->bindInputPort(shading_result);

        SPW::ShaderHandle GBuffer({
                                          "drawGBuffer",
                                          "./resources/shaders/GBuffer.vert",
                                          "./resources/shaders/GBuffer.frag"
                                  });
        SPW::ShaderHandle GBuffer_floor({
                                                "drawFloorGBuffer",
                                                "./resources/shaders/GBuffer.vert",
                                                "./resources/shaders/GBuffer_floor.frag"
                                        });
        SPW::ShaderHandle ani_GBuffer({
                                              "draw_ani_GBuffer",
                                              "./resources/shaders/ani_GBuffer.vert",
                                              "./resources/shaders/GBuffer.frag"
                                      });
        graphs.insert({kDefferShadingGraph, defferShading});

        nodes.insert({kPointShadowNode, p_shadowMap_node});
        nodes.insert({kDirectionalShadowNode, d_shadowMap_node});
        nodes.insert({kGBufferNode, defferNode});

        all_ports.insert({kPointShadowPort, p_shadowMap_output});
        all_ports.insert({kDirectionalShadowPort, d_shadowMap_output});
        all_ports.insert({kGPositionPort, gPosition});
        all_ports.insert({kGAlbedoPort, gAlbedo});
        all_ports.insert({kGNormalPort, gNormal});
        all_ports.insert({kGMetalRoughnessPort, gMetalRoughnessAO});
        all_ports.insert({kGDepthPort, gDepth});

        shaders.insert({kPointShadowShader, SPW::P_shadowmap_desc()});
        shaders.insert({kDirectionalShadowShader, SPW::D_shadowmap_desc()});
        shaders.insert({kAniPointShadowShader, SPW::P_ani_shadowmap_desc()});
        shaders.insert({kAniDirectionalShadowShader, SPW::D_ani_shadowmap_desc()});
        shaders.insert({kGBufferShader, SPW::GBuffer_desc(GBuffer)});
        shaders.insert({kAniGBufferShader, SPW::ani_GBuffer_desc(ani_GBuffer)});
        shaders.insert({kFloorGBufferShader, SPW::GBuffer_desc(GBuffer_floor)});
        shaders.insert({kDefferShadingShader, pbr_deffer_shading_desc});
        shaders.insert({kSSRShader, SSR_desc});
        shaders.insert({kSSRBlurShader, SSR_blur_desc});
    }
}

void SPW::RenderGraphManager::initializeGraph(unsigned int w, unsigned int h) {
    for (auto &[_, graph] : graphs) {
        graph->init(w, h);
    }
}

void SPW::RenderGraphManager::onFrameChanged(unsigned int w, unsigned int h) {
    for (auto &[_, graph] : graphs) {
        graph->onFrameChanged(w, h);
    }
}

void SPW::RenderGraphManager::forEachGraph(const std::function<void(const std::shared_ptr<RenderGraph> &)> &callback) {
    for (auto &[_, graph] : graphs) {
        callback(graph);
    }
}

void SPW::RenderGraphManager::forEachShader(const std::function<void(const ShaderDesc &shader)> &callback) {
    for (auto &[_, shader] : shaders) {
        callback(shader);
    }
}

