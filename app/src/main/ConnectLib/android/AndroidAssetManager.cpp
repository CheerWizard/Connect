//
// Created by cheerwizard on 30.11.24.
//

#include "AssetManager.hpp"
#include "Android.hpp"

Asset AssetManager::open(const char *filepath) {
    AAsset* androidAsset = AAssetManager_open(app->activity->assetManager, filepath, AASSET_MODE_BUFFER);
    return Asset(androidAsset);
}