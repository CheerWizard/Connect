//
// Created by cheerwizard on 29.11.24.
//

#include "AssetManager.hpp"
#include "Application.hpp"

#include <android_native_app_glue.h>

#define ASSET_MANAGER reinterpret_cast<AAssetManager*>(handle)

AssetManager::AssetManager(Application *application) {
    handle = application->app->activity->assetManager;
}

AssetManager::~AssetManager() {
}

Asset AssetManager::open(const char *filepath) {
    AAsset* androidAsset = AAssetManager_open(ASSET_MANAGER, filepath, AASSET_MODE_BUFFER);
    return Asset(androidAsset);
}
