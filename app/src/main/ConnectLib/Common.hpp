//
// Created by cheerwizard on 28.11.24.
//

#ifndef CONNECT_COMMON_HPP
#define CONNECT_COMMON_HPP

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <functional>
#include <pthread.h>
#include <unistd.h>
#include <sstream>
#include <cmath>
#include <random>
#include <atomic>
#include <limits>
#include <memory>
#include <filesystem>
#include <fstream>

using namespace std;

using ID = uint32_t;
inline static ID ID_NULL = UINT32_MAX;

#if defined(ANDROID)

#include <sys/mman.h>
#include <sys/types.h>
#include <android_native_app_glue.h>

inline android_app* app = nullptr;

#endif

#endif //CONNECT_COMMON_HPP