cmake_minimum_required(VERSION 3.22.1)

project("connect")

# print project info
message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message("CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")

# configurations
add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
add_compile_definitions($<$<CONFIG:Debug,Release>:USE_DEFAULT_ALLOCATOR>)
#add_compile_definitions($<$<CONFIG:Debug,Release>:USE_STD_MALLOC>)

include_directories(../cpp)

file(GLOB_RECURSE SRC ../cpp/*.c ../cpp/*.cpp)

macro(build_windows)
    add_library(${PROJECT_NAME} STATIC ${SRC})
    target_link_libraries(${PROJECT_NAME}
            Ws2_32
            Mswsock
            Advapi32
    )
    add_executable(${PROJECT_NAME} WIN32 main.cpp)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME})
endmacro()

macro(build_android)
    add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    # Export ANativeActivity_onCreate(),
    # Refer to: https://github.com/android-ndk/ndk/issues/381.
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=1)

    add_library(${PROJECT_NAME} SHARED ${SRC})

    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)
    target_link_libraries(${PROJECT_NAME}
            android
            log
            native_app_glue
    )
endmacro()

macro(build_linux)
    add_library(${PROJECT_NAME} STATIC ${SRC})
    target_link_libraries(${PROJECT_NAME}
            X11
            Xrandr
            pulse
    )
    add_executable(${PROJECT_NAME} main.cpp)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME})
endmacro()

# defines for OS
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    build_windows()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    build_android()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    build_linux()
endif ()