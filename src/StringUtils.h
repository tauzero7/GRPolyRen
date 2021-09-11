/**
 * File:    StringUtils.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_STRING_UTILS_H
#define GRPR_STRING_UTILS_H

/**
 * @brief Copy string.
 *    Note that 'dest' has to be either 'nullptr' or a valid char string.
 *    In both cases, 'dest' will be deleted.
 * @param src    Pointer to source string.
 * @param dest   Pointer to destination string.
 */
bool CopyString(const char* src, char*& dest);

bool CopyCString(const char* const src, char*& dest);

/**
 * @brief Copy first 'num' characters.
 * @param src
 * @param num
 * @param dest
 * @return
 */
bool CopyNString(const char* src, size_t num, char*& dest);

/**
 * @brief Join file path and file name to fullname according to "%s/%s".
 * @param file_path
 * @param file_name
 * @param fullname
 */
void JoinPathName(const char* file_path, const char* file_name, char*& fullname);

void JoinPathNameEnd(const char* file_path, const char* file_name, const char* file_end, char*& fullname);

/**
 * @brief Replace file ending.
 *   Given a filename like "image.png" which ending ".png" has to be replaced
 *   by the ending ".hist".
 * @param orig_filename     Original filename, e.g. "image.png".
 * @param new_fileending    New file ending, e.g. "hist".
 * @param new_filename      Resulting filename, e.g. "image.hist".
 * @return
 */
bool ReplaceFileEnding(const char* orig_filename, const char* new_fileending, char*& new_filename);

/**
 * @brief Split file path into path and filename.
 * @param filename   Absolute path of file.
 * @param path       Resulting path (must be 'nullptr' or a valid char string.)
 * @param name       Resulting name (must be 'nullptr' or a valid char string.)
 * @return bool
 */
bool SplitFilePath(const char* filename, char*& path, char*& name);

/**
 * @brief Check if string has special ending.
 * @param str   String to test.
 * @param end   Ending to test.
 * @param caseSensitive  Be case sensitive or not.
 * @return
 */
bool StringEndsWith(const char* str, const char* end, bool caseSensitive = true);

bool StringStartsWith(const char* str, const char* start, bool caseSensitive = true);

bool StringIsWindowsFolder(const char* str);

/**
 * @brief Remove leading and trailing spaces.
 * @param text
 */
void TrimString(char*& text);


#endif // GRPR_STRING_UTILS_H
