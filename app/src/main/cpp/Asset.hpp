//
// Created by cheerwizard on 29.11.24.
//

#ifndef CONNECT_ASSET_HPP
#define CONNECT_ASSET_HPP

#include "Types.hpp"

class Asset {

public:
    Asset(void* handle);

    void read();
    void close();

private:
    void* handle = nullptr;
    void* buffer = nullptr;
    size_t size = 0;

};

#endif //CONNECT_ASSET_HPP
