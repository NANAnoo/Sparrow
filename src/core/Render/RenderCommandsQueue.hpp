//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERCOMMANDSQUEUE_HPP
#define SPARROW_RENDERCOMMANDSQUEUE_HPP

#include "RenderCommand.hpp"
#include <vector>

namespace SPW {
    template <class API>
    class RenderCommandsQueue{
    public:
        void executeWithAPI(std::shared_ptr<API> &api) {
            for (auto &command : commands) {
                command.execute(api);
            }
            commands.clear();
        }
        void pushCommand(RenderCommand<API> &&command) {
            commands.emplace_back(command);
        }
    private:
        std::vector<RenderCommand<API>> commands;
    };
}


#endif //SPARROW_RENDERCOMMANDSQUEUE_HPP
