//
// Created by cheerwizard on 04.12.24.
//

#ifndef CONNECT_FILEMANAGER_HPP
#define CONNECT_FILEMANAGER_HPP

class FileManager final {

public:
    static string readFile(const char* filepath);

    static bool writeFile(const char* filepath, const string& str);
    static bool writeFile(const char* filepath, const char* bytes, size_t byteSize);

    static bool createDir(const char* dirName);

    static void copyFile(const char* srcPath, const char* destPath);
    static void copyDirs(const char* srcPath, const char* destPath, bool createRoot = false);

    static string getFullFileName(const char* path);
    static string getFileName(const char* path);

    static bool exists(const char* filepath);

    static string getAbsolutePath(const char* path);

    static string getDirectory(const char* filepath);

    static void getFilepath(const string& fullPath, string& pathWithoutFilename);

};

#endif //CONNECT_FILEMANAGER_HPP
