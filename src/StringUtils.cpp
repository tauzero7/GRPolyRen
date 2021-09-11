/**
 * File:    StringUtils.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include <algorithm>
#include <regex>
#include <stdarg.h>

#include "StringUtils.h"
#include "Utilities.h"


void copy_stdstring(const std::string src, char*& dest)
{
    SafeDelete<char>(dest);

    size_t len = src.size() + 1;
    dest = new char[len];
#ifdef _WIN32
    strcpy_s(dest, len, src.c_str());
#else
    strcpy(dest, src.c_str());
#endif
}

bool CopyString(const char* src, char*& dest)
{
    if (src == nullptr) {
        return false;
    }

    SafeDelete<char>(dest);
    size_t len = strlen(src);

    bool isOkay = true;
#ifdef _WIN32
    dest = new char[len + 4];
    isOkay &= (strncpy_s(dest, len + 4, src, len) == NULL);
#else
    try {
        dest = new char[len + 2];
        isOkay &= (strcpy(dest, src) != nullptr);
    }
    catch (std::bad_alloc&) {
        fprintf(stderr, "CopyString error ... out of memory!\n");
        isOkay = false;
    }
#endif
    return isOkay;
}

bool CopyCString(const char* const src, char*& dest)
{
    if (src == nullptr) {
        return false;
    }

    SafeDelete<char>(dest);
    size_t len = strlen(src);

    bool isOkay = true;
#ifdef _WIN32
    dest = new char[len + 2];
    isOkay &= (strncpy_s(dest, len + 1, src, _TRUNCATE) == NULL);
#else
    try {
        dest = new char[len + 2];
        isOkay &= (strcpy(dest, src) != nullptr);
    }
    catch (std::bad_alloc&) {
        fprintf(stderr, "CopyCString error ... out of memory!\n");
        isOkay = false;
    }
#endif
    return isOkay;
}

bool CopyNString(const char* src, size_t num, char*& dest)
{
    SafeDelete<char>(dest);
    size_t slen = strlen(src);

    bool isOkay = true;
#ifdef _WIN32
    dest = new char[slen + 2]; // TODO!!!!
    isOkay &= (strncpy_s(dest, slen + 1, src, _TRUNCATE) == NULL);
    dest[slen + 1] = '\0';
#else
    size_t len = std::min(slen, num);
    dest = new char[len + 2];
    isOkay &= (strncpy(dest, src, len) != nullptr);
#endif
    return isOkay;
}

void JoinPathName(const char* file_path, const char* file_name, char*& fullname)
{
    SafeDelete<char>(fullname);

    size_t len1 = strlen(file_path);
    size_t len2 = strlen(file_name);
    fullname = new char[len1 + len2 + 4];

    std::string format = "%s/%s";
    if (strcmp(file_path, "") == 0) {
        format = "%s%s";
    }

#ifdef _WIN32
    sprintf_s(fullname, len1 + len2 + 4, format.c_str(), file_path, file_name);
#else
    sprintf(fullname, format.c_str(), file_path, file_name);
#endif
}

void JoinPathNameEnd(const char* file_path, const char* file_name, const char* file_end, char*& fullname)
{
    std::string fname = std::string(file_name) + "." + std::string(file_end);
    JoinPathName(file_path, fname.c_str(), fullname);
}

bool ReplaceFileEnding(const char* orig_filename, const char* new_fileending, char*& new_filename)
{
    std::string filename = std::string(orig_filename);
    size_t pos = filename.find_last_of(".");
    if (pos == std::string::npos) {
        return false;
    }

    filename = filename.substr(0, pos);
    filename = filename.append(new_fileending);

    CopyString(filename.c_str(), new_filename);
    return true;
}

bool SplitFilePath(const char* filename, char*& path, char*& name)
{
    std::string fname = std::string(filename);
    std::string mpath = std::string();
    std::string mname = std::string();

#ifdef _WIN32
    size_t offset = fname.rfind("\\");
    if (offset != std::string::npos) {
        mpath = fname.substr(0, offset) + "\\";
        mname = fname.substr(offset + 1, fname.length() - offset);
    }
    else {
        offset = fname.rfind("/");
        if (offset != std::string::npos) {
            mpath = fname.substr(0, offset) + "/";
            mname = fname.substr(offset + 1, fname.length() - offset);
        }
        else {
            mname = std::string(filename);
        }
    }
#else
    size_t offset = fname.rfind("/");
    if (offset == std::string::npos) {
        mpath = std::string("");
        mname = std::string(filename);
    }
    else {
        mpath = fname.substr(0, offset) + "/";
        mname = fname.substr(offset + 1, fname.length() - offset);
    }
#endif

    copy_stdstring(mpath, path);
    copy_stdstring(mname, name);
    return true;
}

bool StringEndsWith(const char* str, const char* end, bool caseSensitive)
{
    if (str == nullptr || end == nullptr) {
        return false;
    }

    std::string s_str = std::string(str);
    std::string s_end = std::string(end);

    if (!caseSensitive) {
        std::transform(s_str.begin(), s_str.end(), s_str.begin(), ::tolower);
        std::transform(s_end.begin(), s_end.end(), s_end.begin(), ::tolower);
    }

    if (s_str.size() == 0 || s_end.size() == 0 || s_end.size() > s_str.size()) {
        return false;
    }

    size_t elen = s_end.size();
    return (s_str.compare(s_str.size() - elen, elen, s_end) == 0);
}

bool StringStartsWith(const char* str, const char* start, bool caseSensitive)
{
    if (str == nullptr || start == nullptr) {
        return false;
    }

    std::string s_str = std::string(str);
    std::string s_start = std::string(start);

    if (!caseSensitive) {
        std::transform(s_str.begin(), s_str.end(), s_str.begin(), ::tolower);
        std::transform(s_start.begin(), s_start.end(), s_start.begin(), ::tolower);
    }

    if (s_str.size() == 0 || s_start.size() == 0 || s_start.size() > s_str.size()) {
        return false;
    }

    size_t elen = s_start.size();
    return (s_str.compare(0, elen, s_start) == 0);
}

bool StringIsWindowsFolder(const char* str)
{
    std::string sstr = std::string(str);
    std::regex r("[A-Za-z]{1}:[/|\\]", std::regex::extended);
    std::smatch m;
    std::regex_search(sstr, m, r);
    return (m.size() == 1);
}

void TrimString(char*& text)
{
    if (text == nullptr) {
        return;
    }

    std::string stext = std::string(text);
    if (stext.empty()) {
        return;
    }

#if __GNUC__ >= 5
    stext = std::regex_replace(stext, std::regex("^ +| +$|( ) +"), "$1");
#else
    const auto strBegin = stext.find_first_not_of(" ");
    if (strBegin == std::string::npos) {
        return;
    }

    const auto strEnd = stext.find_last_not_of(" ");
    const auto strRange = strEnd - strBegin + 1;

    stext = stext.substr(strBegin, strRange);
#endif

    delete[] text;

    size_t len = stext.size() + 1;
    text = new char[len];
#ifdef _WIN32
    strcpy_s(text, len, stext.c_str());
#else
    strcpy(text, stext.c_str());
#endif
}
