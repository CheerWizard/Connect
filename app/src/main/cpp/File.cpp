//
// Created by mecha on 29.11.2024.
//

#include "File.hpp"

static constexpr int FileModes[FileMode_COUNT] = {
        O_RDONLY,             // FileMode_READ
        O_CREAT | O_WRONLY,   // FileMode_WRITE
        O_CREAT | O_RDWR,     // FileMode_READWRITE
};

static constexpr int FilePermissions[FileMode_COUNT] = {
        S_IRUSR,              // FileMode_READ
        S_IWUSR,              // FileMode_WRITE
        S_IRUSR | S_IWUSR,    // FileMode_READWRITE
};

static constexpr int FileShareModes[FileShareMode_COUNT] = {
        0,                    // FileShareMode_NONE
        S_IROTH,              // FileShareMode_READ
        S_IWOTH,              // FileShareMode_WRITE
        S_IROTH | S_IWOTH,    // FileShareMode_DELETE
};

void File::open(const char *filepath, FileMode mode) {

}

void File::close() {

}

void File::create(const char *filepath, FileMode mode, FileShareMode share_mode) {

}

void File::write(const void *buffer, size_t size) {

}

void File::read(void *buffer, size_t size) {

}

bool File::remove(const char *filepath) {
    return false;
}

bool File::copy(const File &file) {
    return false;
}

bool File::move(const File &file) {
    return false;
}

bool File::createDir(const char *filepath, FileMode mode, FileShareMode share_mode) {
    return false;
}

bool File::isDir(const char *filepath) {
    return false;
}

bool File::isOpen() const {
    return handle == 0;
}

void File::open(const char* filepath, FileMode mode) {
    return open(filepath, FileModes[mode], FilePermissions[mode]);
}

void File::close() {
    close();
}

void File::write(const void *buffer, size_t size) {
    write(buffer, size);
}

void File::read(void* buffer, size_t size) {
    read(buffer, size);
}

void File::create(const char* filepath, FileMode mode, FileShareMode shareMode) {
    return open(filepath, FileModes[mode], FilePermissions[mode] | FileShareModes[shareMode]);
}

bool File::remove(const char *filepath) {
    return remove(filepath);
}

bool File::copy(const File &file) {
}

bool File::move(const File &file) {
}

FileError File::createDir(const char* filepath, FileMode mode, FileShareMode shareMode) {
    const int r = mkdir(filepath, FilePermissions[mode]);
    FileError e = FileError_NONE;
    switch (r) {
        case EACCES: {
            e = FileError_NONE;
            break;
        }
        case ELOOP: {

        }
    }
    return e;
}

bool file_is_dir(const char* filepath) {
    struct stat path_stat = {};
    stat(filepath, &path_stat);
    return S_ISREG(path_stat.st_mode);
}