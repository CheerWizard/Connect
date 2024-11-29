//
// Created by mecha on 29.11.2024.
//

#ifndef CONNECT_FILE_HPP
#define CONNECT_FILE_HPP

#include "Types.hpp"

enum FileMode {
    FileMode_READ,
    FileMode_WRITE,
    FileMode_READWRITE,

    FileMode_COUNT
};

enum FileShareMode {
    FileShareMode_NONE,
    FileShareMode_READ,
    FileShareMode_WRITE,
    FileShareMode_DELETE,

    FileShareMode_COUNT
};

enum FileError {
    FileError_NONE,
    FileError_ACCESS,
    FileError_EXIST,
    FileError_LOOP,
    FileError_MLINK,
    FileError_NAME_TOO_LONG,
    FileError_NO_ENT,
    FileError_NOT_DIR,
    FileError_READ_ONLY_FILESYSTEM,

    FileError_COUNT
};

class File {
    friend File;

public:
    void open(const char* filepath, FileMode mode);
    void close();
    void create(const char* filepath, FileMode mode, FileShareMode share_mode = FileShareMode::NONE);
    void write(const void* buffer, size_t size);
    void read(void* buffer, size_t size);
    bool remove(const char* filepath);
    bool copy(const File& file);
    bool move(const File& file);
    bool createDir(const char* filepath, FileMode mode, FileShareMode share_mode = FileShareMode::NONE);
    bool isDir(const char* filepath);
    bool isOpen() const;

private:
    int handle = 0;
};

#endif //CONNECT_FILE_HPP
