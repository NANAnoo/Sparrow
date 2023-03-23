// #include "ComponentWrapper.hpp"

// #include "EcsFramework/Component/BasicComponent/IDComponent.h"
// #include "EcsFramework/Component/BasicComponent/NameComponent.h"
// #include "EcsFramework/Component/Audio/AudioComponent.h"
// #include "EcsFramework/Component/Audio/AudioListener.h"
// #include "EcsFramework/Component/CameraComponent.hpp"
// #include "EcsFramework/Component/KeyComponent.hpp"
// #include "EcsFramework/Component/MouseComponent.hpp"
// #include "EcsFramework/Component/LightComponent.hpp"
// #include "EcsFramework/Component/TransformComponent.hpp"

// #define GET_COMP(comp) m_entity->has<comp>() ? m_entity->component<comp>(): m_entity->emplace<comp>()

// namespace SPW {

//     template<> void ComponentWrapper::setupComponent<IDComponent>(const sol::table &value) {

//     }
//     template<> void ComponentWrapper::setupComponent<NameComponent>(const sol::table &value) {
        
//     }
//     template<> void ComponentWrapper::setupComponent<CameraComponent>(const sol::table &value) {
        
//     }
//     template<> void ComponentWrapper::setupComponent<MouseComponent>(const sol::table &value) {
        
//     }
//     template<> void ComponentWrapper::setupComponent<LightComponent>(const sol::table &value) {
        
//     }
//     template<> void ComponentWrapper::setupComponent<TransformComponent>(const sol::table &value) {
//         auto trans = GET_COMP(TransformComponent);
//         trans->position = value["position"];
//         trans->rotation = value["rotation"];
//         trans->scale = value["scale"];
//     }
//     template<> void ComponentWrapper::setupComponent<AudioComponent>(const sol::table &value) {
        
//     }
//     template<> void ComponentWrapper::setupComponent<AudioListener>(const sol::table &value) {
        
//     }
// }