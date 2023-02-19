//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERCOMMANDSQUEUE_HPP
#define SPARROW_RENDERCOMMANDSQUEUE_HPP

#include "RenderCommand.hpp"
#include "RenderAPII.h"

namespace SPW {
    class RenderCommandsQueue{
    public:
        void executeWithAPI(RenderAPII &api) {
            for (auto &command : commands) {
                command.execute(api);
            }
            commands.clear();
        }
        void pushCommand(const RenderCommand &command) {
            commands.emplace_back(command);
        }
    private:
        std::vector<RenderCommand> commands;
    };
}


#endif //SPARROW_RENDERCOMMANDSQUEUE_HPP
