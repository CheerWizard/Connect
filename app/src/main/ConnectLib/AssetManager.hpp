//
// Created by cheerwizard on 29.11.24.
//

#ifndef CONNECT_ASSETMANAGER_HPP
#define CONNECT_ASSETMANAGER_HPP

using AssetHandle = void*;

class AssetManager {

public:
    AssetHandle open(const char* filepath);
    void close(AssetHandle assetHandle);
    void read(AssetHandle assetHandle, void*& buffer, size_t& size);

};

#endif //CONNECT_ASSETMANAGER_HPP
