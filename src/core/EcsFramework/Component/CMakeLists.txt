add_subdirectory(BasicComponent)
add_subdirectory(Audio)
add_subdirectory(PhysicalComponent)

CORE_ADD_SOURCE(ComponentI.h
        TransformComponent.hpp
        LightComponent.hpp
        CameraComponent.hpp
        ModelComponent.h
        KeyComponent.hpp
        MouseComponent.hpp)