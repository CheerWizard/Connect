//
// Created by cheerwizard on 29.11.24.
//

#include "Asset.hpp"
#include "Logger.hpp"

#include <android/asset_manager.h>

#define ASSET reinterpret_cast<AAsset*>(handle)

Asset::Asset(void *handle)
:
handle(handle)
{
}

void Asset::read() {
    buffer = (void*) AAsset_getBuffer(ASSET);
    size = AAsset_getLength(ASSET);
}

void Asset::close() {
    LOG_INFO("close()");
    AAsset_close((AAsset*) handle);
}