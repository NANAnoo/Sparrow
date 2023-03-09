//
// Created by ZhangHao on 2023/2/15.
//

#ifndef SPARROW_COMPONENT_I_H
#define SPARROW_COMPONENT_I_H

#include <type_traits>

namespace SPW {
    class ComponentI {

    };

    template <class T>
    concept Component = std::is_base_of_v<ComponentI, T>;

    template <Component ...components>
    struct ComponentGroup
    {
    };

}

#endif //SPARROW_COMPONENT_I_H
