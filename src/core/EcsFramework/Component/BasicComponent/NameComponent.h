//
// Created by ZhangHao on 2023/2/15.
//

#ifndef SPARROW_NAMECOMPONENT_H
#define SPARROW_NAMECOMPONENT_H

#include "../ComponentI.h"

#include <string>

namespace SPW {
    // give name to an entity
    class NameComponent : public ComponentI {
    public:
        NameComponent() {
            name = "Unknown";
            std::cout<< name << std::endl;
        };
        explicit NameComponent(const std::string &aName) {
            name = aName;
        }
        [[nodiscard]] const std::string &getName() const {
            return name;
        }
        void updateName(const std::string &aName) {
            name = aName;
        }
    private:
        std::string name;
    };
}


#endif //SPARROW_NAMECOMPONENT_H
