//
// Created by skymelody on 2019/12/24.
//

#include <vector>
#include "Shark2File/Common/FileUtils.h"

#if defined(__linux__)
extern "C" {
#include <unistd.h>
#include <sys/stat.h>
}

#elif defined(_WIN32)
extern "C" {
#include <Windows.h>
#include <direct.h>
}
#endif

#define PATH_MAX_LEN 1024


std::string FileSystem::Current() {
    char buf[PATH_MAX_LEN]{0};
#ifdef __linux__
    getcwd(buf, PATH_MAX_LEN);
#else
    _getcwd(buf, PATH_MAX_LEN);
#endif
    return std::string(buf);
}

bool FileSystem::Exist(const std::string& dirPath) {
#ifdef __linux__
    struct stat st;
    return stat(dirPath.c_str(), &st) == 0;
#else
    DWORD attr = GetFileAttributes(dirPath.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        switch(GetLastError()) {
            case ERROR_FILE_NOT_FOUND:
            case ERROR_PATH_NOT_FOUND:
            case ERROR_NOT_READY:
            case ERROR_INVALID_DRIVE:
                return false;
            default:
                return false;
        }
    }
    return true;
#endif
}

bool FileSystem::IsDir(const std::string &dirPath) {
    if (dirPath.empty()) {
        return false;
    }

#ifdef __linux__
    struct stat st;
    if (stat(dirPath.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    else {
        return false;
    }
#else
    DWORD attr = GetFileAttributes(dirPath.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
#endif
}

bool FileSystem::MakeDir(std::string path) {
    if (path.empty()) {
        return false;
    }
    if (path.back() != PATH_SEPARATOR[0]) {
        path.append(PATH_SEPARATOR);
    }

    std::vector<std::string> dirs;
    int pos = -1;
    while ((pos = path.find(PATH_SEPARATOR, pos + 1)) != std::string::npos) {
        std::string tmp = path.substr(0, pos);
        if (!tmp.empty()) {
            dirs.push_back(tmp);
        }
    }
#ifdef __linux__
    for (const std::string& dir : dirs) {
        struct stat st;
        if (stat(dir.c_str(), &st) != 0) {
            if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
                return false;
            }
        }
    }
#else
    for (const std::string& dir : dirs) {
        DWORD attr = GetFileAttributes(dir.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES &&
           (GetLastError() == ERROR_FILE_NOT_FOUND ||
            GetLastError() == ERROR_PATH_NOT_FOUND ||
            GetLastError() == ERROR_NOT_READY ||
            GetLastError() == ERROR_INVALID_DRIVE)) {
            if (CreateDirectoryA(dir.c_str(), 0) == 0) {
                return false;
            }
        }
    }
#endif
    return true;
}

std::string FileSystem::GenerateDir(const std::string &root,
                                    const std::string &packetType,
                                    const std::string &ip,
                                    bool src) {
    return root +
           PATH_SEPARATOR +
           packetType +
           PATH_SEPARATOR +
           (src ? "src" : "dest") +
           PATH_SEPARATOR +
           ip;
}

