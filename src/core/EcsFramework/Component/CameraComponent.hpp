//
// Created by ZhangHao on 2023/2/20.
//

#pragma once

#include "ComponentI.h"

namespace SPW {
    //  camera type, used for building ProjectionMatrix
    // default direction is (0, 0, -1)
    enum CameraType {
        PerspectiveType, // Perspective camera
        OrthoType, // Orthographic camera
        UIOrthoType // only used for UI, still an ortho camera
    };
    class CameraComponent : public ComponentI {
    public:
        CameraComponent() = delete;
        explicit CameraComponent(CameraType type) : cameraType(type) {}
        // PerspectiveType
        float fov = 1.0;
        float aspect = 1.0;
        float near = 0.1;
        float far = 10;

        // ortho type
        float left = 0;
        float right = 0;
        float bottom = 1;
        float top = 1;
        [[nodiscard]] CameraType getType() const { return cameraType;}
    private:
        CameraType cameraType = PerspectiveType;
    };
}
