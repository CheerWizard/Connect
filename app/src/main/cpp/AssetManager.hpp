//
// Created by cheerwizard on 29.11.24.
//

#ifndef CONNECT_ASSETMANAGER_HPP
#define CONNECT_ASSETMANAGER_HPP

#include "Asset.hpp"

class Application;

class AssetManager {

public:
    AssetManager(Application* application);
    ~AssetManager();

    Asset open(const char* filepath);

private:
    void* handle = nullptr;

};

#endif //CONNECT_ASSETMANAGER_HPP
