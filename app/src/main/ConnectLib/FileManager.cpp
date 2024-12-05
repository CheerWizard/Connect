//
// Created by cheerwizard on 04.12.24.
//

#include <opencl-c-base.h>
#include "FileManager.hpp"

bool FileManager::createDir(const char* dirName) {
    return filesystem::create_directory(dirName);
}

void FileManager::getFilepath(
        const string& fullPath,
        string& pathWithoutFilename
)
{
    // Remove the file name and store the path to this folder
    size_t found = fullPath.find_last_of("/\\");
    pathWithoutFilename = fullPath.substr(0, found + 1);
}

string FileManager::readFile(const char* filepath)
{
    ifstream file;
    file.open(filepath);
    if (!file.is_open()) {
        LOG_ERR("Failed to open to read a file {}", filepath);
        return {};
    }

    string line;
    string buffer;
    while (getline(file, line)) {
        buffer += line + '\n';
    }

    file.close();
    return buffer;
}

bool FileManager::writeFile(const char* filepath, const string &str)
{
    return writeFile(filepath, str.c_str(), str.length());
}

bool FileManager::writeFile(const char* filepath, const char* bytes, size_t byteSize)
{
    ofstream file;
    file.open(filepath);
    if (!file.is_open()) {
        LOG_ERR("Failed to open to write a file {}", filepath);
        return false;
    }
    file.write(bytes, byteSize);
    file.close();
    return true;
}

void FileManager::copyFile(const char *srcPath, const char *destPath)
{
    if (!filesystem::exists(destPath)) {
        writeFile(destPath, "");
    }
    filesystem::copy_file(srcPath, destPath, filesystem::copy_options::overwrite_existing);
}

string FileManager::getFullFileName(const char *path)
{
    filesystem::path filepath = path;
    if (filepath.has_filename()) {
        return filepath.filename().string();
    }
    return {};
}

string FileManager::getFileName(const char* path)
{
    // todo remove extension from file name
    filesystem::path filepath = path;
    if (filepath.has_filename()) {
        return filepath.filename().string();
    }
    return {};
}

void FileManager::copyDirs(const char* srcPath, const char* destPath, bool createRoot)
{
    if (createRoot) {
        filesystem::create_directory(destPath);
    }

    for (filesystem::path p: filesystem::directory_iterator(srcPath))
    {
        filesystem::path destFile = destPath/p.filename();

        if (filesystem::is_directory(p)) {
            filesystem::create_directory(destFile);
            copyDirs(p.string().c_str(), destFile.string().c_str(), false);
        }

        else {

            try {

                if (filesystem::exists(destFile)) {
                    filesystem::copy_file(p, destFile, filesystem::copy_options::overwrite_existing);
                }

                else {
                    filesystem::copy(p, destFile);
                }

            } catch (exception& e) {
                LOG_ERR("Failed to copy dirs from {} to {} \n Error: {}",
                         p.string(),
                         destFile.string(),
                         e.what());
            }

        }
    }
}

bool FileManager::exists(const char *filepath)
{
    return filesystem::exists(filepath);
}

string FileManager::getAbsolutePath(const char* path)
{
    return filesystem::absolute(path).string();
}

string FileManager::getDirectory(const char *filepath)
{
    string filepathStr = filepath;
    return filepathStr.substr(0, filepathStr.find_last_of('/'));
}