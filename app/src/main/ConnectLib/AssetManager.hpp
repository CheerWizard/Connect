//
// Created by cheerwizard on 29.11.24.
//

#ifndef CONNECT_ASSETMANAGER_HPP
#define CONNECT_ASSETMANAGER_HPP

#include "Asset.hpp"

class AssetManager {

public:
    static Asset open(const char* filepath);

};

#endif //CONNECT_ASSETMANAGER_HPP
