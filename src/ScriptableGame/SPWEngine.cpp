//
// Created by NANAnoo on 3/20/2023.
//

#include "GameWrapper.hpp"
#include "IO/LogSystem/LogSystem.hpp"


// main entrance
int main(int argc, char **argv) {

	if (SPW::ConfigManager::Boost())
    {
        TEST_LOGGER_INFO("Boot Lua Test")
    }
    else return -1;


    auto scriptPath = SPW::ConfigManager::GetScriptPath();
    if (!scriptPath.has_value())
        return -1;

    std::string path = scriptPath.value();

	auto appProxy = SPW::Application::create<SPW::GameWrapper>(path);

    return appProxy->app->run(argc, argv);
}