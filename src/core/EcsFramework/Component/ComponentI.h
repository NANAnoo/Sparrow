//
// Created by ZhangHao on 2023/2/15.
//

#ifndef SPARROW_COMPONENT_I_H
#define SPARROW_COMPONENT_I_H

#include <type_traits>
#include <sol/sol.hpp>

namespace SPW {
    class ComponentI {
        public:
        virtual void update(const std::string&key, const sol::table &value) {
            std::printf("component update not implement\n");
        }

        virtual void initFromLua(const sol::table &value) {
            std::printf("initFromLua not implement\n");
        }
    };

    template <class T>
    concept Component = std::is_base_of_v<ComponentI, T>;

    template <Component ...components>
    struct ComponentGroup
    {
    };

}

#endif //SPARROW_COMPONENT_I_H
