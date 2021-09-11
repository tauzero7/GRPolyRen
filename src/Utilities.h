/**
 * File:    Utilities.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_UTILITIES_H
#define GRPR_UTILITIES_H

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#undef max
#endif //

template <typename T> T Clamp(const T value, T minValue, T maxValue)
{
    T result = value;
    if (value < minValue) {
        result = minValue;
    }
    else if (value > maxValue) {
        result = maxValue;
    }
    return result;
}

bool DirExists(const char* pathname);

/**
 * @brief Test if file exists.
 * @param filename   Name of file to be tested.
 * @return
 */
bool FileExists(const char* filename);

/**
 * @brief GetExePath
 * @param path
 * @return
 */
bool GetExePath(char*& path);

// move to string utils
bool GetFilePath(const char* filename, char*& path);

/**
 * @brief IsBitSet
 * @param val
 * @param bit
 * @return
 */
bool IsBitSet(int val, int bit);

/**
 * @brief Set bit.
 * @param val  Integer val represents a bit field.
 * @param bit  Bit must be (1,2,4,8,16,...)
 */
void SetBit(int& val, int bit);

/**
 * @brief ClearBit
 * @param val
 * @param bit
 */
void ClearBit(int& val, int bit);

/**
 * @brief ConvertBits
 * @param bits
 * @param bytes
 */
void ConvertBits(char bits, unsigned char* bytes);

/**
 * @brief Safely delete 1D arrays generated with 'new'.
 *
 *  Each pointer to a 1D array should be immediately
 *  initialized or set to nullptr. This method checks
 *  if pointer is not 'nullptr', deletes the array,
 *  and sets the pointer to 'nullptr'.
 *
 * @tparam T  Pointer to data array.
 */
template <typename T> void SafeDelete(T*& ptr)
{
    if (ptr != nullptr) {
        delete[] ptr;
        ptr = nullptr;
    }
}

/**
 * @brief Safely delete class generated with 'new'.
 *
 * @tparam C  Pointer to class.
 */
template <typename C> void SafeDtor(C*& cptr)
{
    if (cptr != nullptr) {
        delete cptr;
        cptr = nullptr;
    }
}

#endif // GRPR_UTILITIES_H
