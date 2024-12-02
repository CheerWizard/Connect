cmake_minimum_required(VERSION 3.22.1)

project("connect")

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
set(CMAKE_CXX_FLAGS "-DVK_USE_PLATFORM_ANDROID_KHR")

file(GLOB_RECURSE SRC ../cpp/*.c ../cpp/*.cpp)

include_directories(../cpp)

add_library(${CMAKE_PROJECT_NAME} SHARED ${SRC})

include(FetchContent)

FetchContent_Declare(
        oboe
        GIT_REPOSITORY https://github.com/google/oboe.git
)

FetchContent_MakeAvailable(oboe)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        native_app_glue
        EGL
        GLESv1_CM
        oboe
)