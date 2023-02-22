//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERCOMMANDSQUEUE_HPP
#define SPARROW_RENDERCOMMANDSQUEUE_HPP

#include "RenderCommand.hpp"
#include "RenderBackEndI.h"

namespace SPW {
    class RenderCommandsQueue{
    public:
        void executeWithAPI(std::shared_ptr<RenderBackEndI> &backEnd) {
            for (auto &command : commands) {
                command.execute(backEnd);
            }
            commands.clear();
        }
        void pushCommand(RenderCommand &&command) {
            commands.emplace_back(command);
        }
    private:
        std::vector<RenderCommand> commands;
    };
}


#endif //SPARROW_RENDERCOMMANDSQUEUE_HPP
