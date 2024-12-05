//
// Created by cheerwizard on 29.11.24.
//

#ifndef CONNECT_ASSETMANAGER_HPP
#define CONNECT_ASSETMANAGER_HPP

using AssetHandle = void*;

class AssetManager {

public:
    static AssetHandle open(const char* filepath);
    static void close(AssetHandle assetHandle);
    static void read(AssetHandle assetHandle, void*& buffer, size_t& size);

};

#endif //CONNECT_ASSETMANAGER_HPP
