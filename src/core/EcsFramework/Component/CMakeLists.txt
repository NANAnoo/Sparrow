add_subdirectory(BasicComponent)
add_subdirectory(AnimationComponent)
add_subdirectory(Audio)
add_subdirectory(PhysicalComponent)
add_subdirectory(Lights)

CORE_ADD_SOURCE(ComponentI.h
        MeshComponent.hpp
        TransformComponent.hpp
        CameraComponent.hpp
        ModelComponent.h
        KeyComponent.hpp
        MouseComponent.hpp
        AnimationComponent/AnimationComponent.h
        )