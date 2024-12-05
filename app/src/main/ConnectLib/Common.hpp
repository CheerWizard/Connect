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
#include <sys/socket.h>
#include <android/api-level.h>
#include <android_native_app_glue.h>
#include <android/sensor.h>
#include <android/input.h>
#include <android/choreographer.h>
#include <android/set_abort_message.h>
#include <android/log.h>
#include <android/AndroidGLES.hpp>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

inline android_app* app = nullptr;
inline EGLContext graphicsContext;

#endif

#endif //CONNECT_COMMON_HPP