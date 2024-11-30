//
// Created by cheerwizard on 30.11.24.
//

#include "Asset.hpp"
#include "Logger.hpp"
#include "android/AndroidLauncher.hpp"

#define ASSET reinterpret_cast<AAsset*>(handle)

void Asset::read() {
    buffer = (void*) AAsset_getBuffer(ASSET);
    size = AAsset_getLength(ASSET);
}

void Asset::close() {
    LOG_INFO("close()");
    AAsset_close((AAsset*) handle);
}