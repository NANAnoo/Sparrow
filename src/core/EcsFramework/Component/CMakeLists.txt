add_subdirectory(BasicComponent)
add_subdirectory(Audio)
add_subdirectory(Lights)

CORE_ADD_SOURCE(ComponentI.h
        TransformComponent.hpp
        CameraComponent.hpp
        ModelComponent.h
        KeyComponent.hpp
        MouseComponent.hpp)