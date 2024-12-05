//
// Created by cheerwizard on 30.11.24.
//

#include "AssetManager.hpp"

AssetHandle AssetManager::open(const char *filepath) {
    return AAssetManager_open(app->activity->assetManager, filepath, AASSET_MODE_BUFFER);
}

void AssetManager::close(AssetHandle assetHandle) {

}

void AssetManager::read(AssetHandle assetHandle, void *&buffer, size_t &size) {

}

void AssetManager::write(AssetHandle assetHandle, void *buffer, size_t size) {

}
