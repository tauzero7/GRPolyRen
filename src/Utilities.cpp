/**
 * File:    Utilities.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "Utilities.h"
#include "StringUtils.h"

#include <algorithm>
#include <ctime>
#include <regex>
#include <string>
#include <vector>

#ifdef _WIN32
#include <bitset>
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

bool GetExePath(char*& path)
{
    SafeDelete<char>(path);

#ifdef _WIN32
    const unsigned int nSize = MAX_PATH;
#else
    const unsigned int nSize = 0xFFFF;
#endif

    char* cwd = new char[nSize];
#ifdef _WIN32
    if (::GetModuleFileNameA(NULL, cwd, nSize) == ERROR_INSUFFICIENT_BUFFER) {
        cwd[0] = 0;
    }
    else {
        if (::GetLastError() != ERROR_SUCCESS) {
            cwd[0] = 0;
        }
    }
#elif defined(__APPLE__)
    char apath[1024];
    uint32_t asize = sizeof(apath);
    if (_NSGetExecutablePath(apath, &asize) == 0) {
        realpath(apath, cwd);
    }
    else {
        cwd[0] = 0;
    }
#else
    char* tmp = new char[nSize];
    sprintf(tmp, "/proc/%d/exe", getpid());
    ssize_t size = readlink(tmp, cwd, nSize - 1);
    if (size >= 0) {
        cwd[size] = 0;
    }
    else {
        cwd[0] = 0;
    }
    delete[] tmp;
#endif

    size_t cwdlen = static_cast<size_t>(strlen(cwd)) + 1;
    path = new char[cwdlen];

#ifdef _WIN32
    strcpy_s(path, cwdlen, cwd);
#else
    strcpy(path, cwd);
#endif

    // remove exe name
    GetFilePath(cwd, path);

    bool isOkay = true;
    if (cwd[0] == 0) {
        fprintf(stderr, "Cannot determine current working directory!\n");
        isOkay = false;
    }
    delete[] cwd;
    return isOkay;
}

bool GetFilePath(const char* filename, char*& path)
{
    std::string fname = std::string(filename);
    std::string mpath = std::string();

#ifdef _WIN32
    size_t offset = fname.rfind("\\");
    if (offset != std::string::npos) {
        mpath = fname.substr(0, offset) + "\\";
    }
    else {
        offset = fname.rfind("/");
        if (offset != std::string::npos) {
            mpath = fname.substr(0, offset) + "/";
        }
    }
#else
    size_t offset = fname.rfind("/");
    if (offset == std::string::npos) {
        mpath = std::string("");
    }
    else {
        mpath = fname.substr(0, offset) + "/";
    }
#endif

    CopyString(mpath.c_str(), path);
    return true;
}

bool DirExists(const char* pathname)
{
    if (pathname == nullptr || strlen(pathname) == 0 || strcmp(pathname, "") == 0) {
        return false;
    }

    struct stat buf;
    return (stat(pathname, &buf) == 0);
}

bool FileExists(const char* filename)
{
    // This method could be replaced if going to c++-14 or c++-17
    //  C++-14 :  std::experimental::filesystem::exists("helloworld.txt");
    //  C++-17 :  std::filesystem::exists("helloworld.txt");
    //
    if (filename == nullptr || strlen(filename) == 0 || strcmp(filename, "") == 0) {
        return false;
    }

#if 1
    std::ifstream ifs;
    ifs.open(filename, std::ios_base::in);
    if (ifs.good()) {
        ifs.close();
        return true;
    }
    return false;
#else
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
#endif
}

bool IsBitSet(int val, int bit)
{
    return ((val & bit) == bit);
}

void SetBit(int& val, int bit)
{
    val |= bit;
}

void ClearBit(int& val, int bit)
{
    val &= ~bit;
}

void ConvertBits(char bits, unsigned char* bytes)
{
    std::bitset<8> bs(bits);
    for (size_t i = 0; i < 8; i++) {
        bytes[7 - i] = static_cast<unsigned char>(bs.test(i) ? 0 : 1);
    }
}
