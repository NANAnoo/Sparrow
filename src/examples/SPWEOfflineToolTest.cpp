//
// Created by dudu on 2023/4/20.
//
#include <iostream>
#include "Asset/AssetManager/AssetManager.h"
#include "IO/ConfigManager.h"

// main entrance
int main(int argc, char** argv)
{
    if (SPW::ConfigManager::ReadConfig())
        std::cout << "Successfully read config file" << std::endl;

    SPW::FileSystem::MountFromConfig();

    SPW::AssetManager::ImportModel("./resources/models/scene/scene.gltf");
    SPW::AssetManager::ImportModel("./resources/models/mantis/mantis.gltf");
    SPW::AssetManager::ImportModel("./resources/models/cube.obj");
    SPW::AssetManager::ImportModel("./resources/models/sand_cube/sand_cube.obj");
    SPW::AssetManager::ImportModel("./resources/models/dragon/dragon.gltf");
	return 0;
}
