//
// Created by ZhangHao on 2023/2/20.
//

#pragma once

#include "ComponentI.h"
#include "IO/EntitySerializer.h"

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

        //whether main
        bool whetherMainCam = false;

        SaveTable save()
        {
          auto tbl = SaveTable
              {
                  {"fov", fov},
                  {"aspect", aspect},
                  {"left", left},
                  {"right", right},
                  {"bottom", bottom},
                  {"top", top},
                  {"CameraType", static_cast<int>(getType())},
                  {"whetherMainCam", whetherMainCam},
              };
          return tbl;
        }

        CameraComponent& load(SaveTable&& tbl)
        {
          CameraComponent camera_component{CameraType::PerspectiveType};
          for (const auto& [k, v] : tbl)
          {
            std::cout << k << std::endl;
            const auto& flied_name = std::string(k);
            if (flied_name == "CameraType")
            {
              *v.as_integer();
            }
            if (flied_name == "fov")
            {
              *v.as_floating_point();
            }
            if (flied_name == "aspect")
            {
              *v.as_floating_point();
            }
            if (flied_name == "near")
            {
              *v.as_floating_point();
            }
            if (flied_name == "far")
            {
              *v.as_floating_point();
            }
            if (flied_name == "left")
            {
              *v.as_floating_point();
            }
            if (flied_name == "right")
            {
              *v.as_floating_point();
            }
            if (flied_name == "bottom")
            {
              *v.as_floating_point();
            }
            if (flied_name == "top")
            {
              *v.as_floating_point();
            }
            if (flied_name == "whetherMainCam")
            {
              *v.as_boolean();
            }
          }

          return camera_component;
        }

    private:
        CameraType cameraType = PerspectiveType;
    };
}
