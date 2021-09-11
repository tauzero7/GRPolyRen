/**
 * File:    FileTokenizer.cpp
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#include "FileTokenizer.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>

FileTokenizer::FileTokenizer()
    : m_filename(std::string())
{
    //
}

FileTokenizer::FileTokenizer(const char* filename)
    : m_filename(std::string())
{
    ReadFile(filename);
}

FileTokenizer::FileTokenizer(const char* filename, const char* ignToken)
    : m_filename(std::string())
{
    InsertIgnoreToken(ignToken);
    ReadFile(filename);
}

FileTokenizer::FileTokenizer(const char* filename, char ignToken)
    : m_filename(std::string())
{
    InsertIgnoreToken(ignToken);
    ReadFile(filename);
}

FileTokenizer::~FileTokenizer()
{
    ClearIgnoreTokens();
    ClearTokens();
    m_lines.clear();
}

void FileTokenizer::ClearIgnoreTokens()
{
    m_ignoreTokens.clear();
}

void FileTokenizer::ClearTokens()
{
    m_tokens.clear();
}

int FileTokenizer::FindTokenInLine(const int lineNumber, const char* ftoken) {
    if (!isValid(lineNumber)) {
        return -1;
    }
     
    size_t row = static_cast<size_t>(lineNumber);
    for (size_t c = 0; c < m_tokens[row].size(); c++) {
        if (m_tokens[row][c].compare(ftoken) == 0) {
            return static_cast<int>(c);
        }
    }
    return -1;
}

const char* FileTokenizer::GetLine(const unsigned int lineNumber)
{
    if (lineNumber >= m_lines.size()) {
        return nullptr;
    }

    return m_lines[lineNumber].c_str();
}

unsigned int FileTokenizer::GetNumLines()
{
    return static_cast<unsigned int>(m_tokens.size());
}

int FileTokenizer::GetNumSubTokens(const int lineNumber)
{
    if (lineNumber >= static_cast<int>(m_tokens.size()) || lineNumber < 0) {
        return -1;
    }
    return static_cast<int>(m_tokens[static_cast<size_t>(lineNumber)].size());
}

bool FileTokenizer::GetSubToken(const char* btoken, const int colNumber, char*& stok)
{
    int lineNum, numCols;
    if (!IsBaseToken(btoken, lineNum, numCols)) {
        return false;
    }

    if (colNumber < numCols) {
        return GetSubToken(lineNum, colNumber, stok);
    }
    return false;
}

bool FileTokenizer::GetSubBoolToken(const char* btoken, int colNumber, bool& bval)
{
    int lineNum, numCols;
    if (!IsBaseToken(btoken, lineNum, numCols)) {
        return false;
    }

    if (colNumber < numCols) {
        int itok;
        if (GetSubToken<int>(btoken, colNumber, itok)) {
            bval = (itok == 1);
            return true;
        }
    }

    return false;
}

bool FileTokenizer::GetSubToken(const int lineNumber, int colNumber, char*& stok)
{
    if (!isValid(lineNumber, colNumber)) {
        return false;
    }

    size_t row = static_cast<size_t>(lineNumber);
    size_t col = static_cast<size_t>(colNumber);
    CopyString(m_tokens[row][col].c_str(), stok);
    return true;
}

void FileTokenizer::PrintLine(const int lineNumber, FILE* fptr)
{
    if (!isValid(lineNumber)) {
        return;
    }

    size_t row = static_cast<size_t>(lineNumber);
    for (size_t c = 0; c < m_tokens[row].size(); c++) {
        fprintf(fptr, "%s ", m_tokens[row][c].c_str());
    }
    fprintf(fptr, "\n");
}

bool FileTokenizer::isValid(const int lineNumber)
{
    return (lineNumber >= 0 && lineNumber < static_cast<int>(m_tokens.size()));
}

bool FileTokenizer::isValid(const int lineNumber, int& colNumber)
{
    bool isOkay = (lineNumber >= 0);
    if (isOkay) {
        int numSubTokens = GetNumSubTokens(lineNumber);

        if (colNumber < 0) {
            colNumber = numSubTokens + colNumber;
        }

        isOkay &= (colNumber >= 0);
        isOkay &= (colNumber < numSubTokens);
    }

    return isOkay;
}

void FileTokenizer::InsertIgnoreToken(const char* itoken)
{
    if (itoken == nullptr) {
        return;
    }

    std::string igntoken = std::string(itoken, strlen(itoken));
    auto it = std::find(m_ignoreTokens.begin(), m_ignoreTokens.end(), igntoken);
    if (it == m_ignoreTokens.end()) {
        m_ignoreTokens.push_back(igntoken);
    }
}

void FileTokenizer::InsertIgnoreToken(const char itoken)
{
    std::string igntoken = std::string(1, itoken);
    this->InsertIgnoreToken(igntoken.c_str());
}

bool FileTokenizer::IsBaseToken(const char* token, int& lineNum, int& numCols)
{
    std::string stoken = std::string(token);
    for (size_t i = 0; i < m_tokens.size(); i++) {
        if (stoken.compare(m_tokens[i][0]) == 0) {
            lineNum = static_cast<int>(i);
            numCols = static_cast<int>(m_tokens[i].size());
            return true;
        }
    }
    lineNum = numCols = -1;
    return false;
}

bool FileTokenizer::IsBaseTokenValid(const char* token, unsigned int lineNumber)
{
    if (lineNumber >= m_tokens.size()) {
        return false;
    }

    std::string stoken = std::string(token);
    if (stoken.compare(m_tokens[lineNumber][0]) == 0) {
        return true;
    }
    return false;
}

bool FileTokenizer::IsBaseTokenValid(const char* token, unsigned int lineNumber, int& numCols)
{
    bool isOkay = IsBaseTokenValid(token, lineNumber);
    if (isOkay) {
        numCols = static_cast<int>(m_tokens[lineNumber].size());
    }
    else {
        numCols = 0;
    }
    return isOkay;
}

bool FileTokenizer::ReadFile(const char* filename)
{
    if (filename == nullptr) {
        fprintf(stderr, "FileTokenizer::ReadFile() ... no filename given!\n");
        return false;
    }

    if (!FileExists(filename)) {
        fprintf(stderr, "FileTokenizer::ReadFile '%s' ... file does not exist!\n", filename);
        return false;
    }
    m_filename = std::string(filename, strlen(filename));
    return tokenize();
}

bool FileTokenizer::tokenize()
{
    std::ifstream in(m_filename.c_str());
    if (!in.is_open()) {
        char msg[256];
#ifdef _WIN32
        sprintf_s(msg, "Cannot open file \"%s\"", m_filename.c_str());
#else
        sprintf(msg, "Cannot open file \"%s\"", m_filename.c_str());
#endif
        fprintf(stderr, "%s\n", msg);
        return false;
    }

    ClearTokens();
    m_lines.clear();

    while (!in.eof() && !in.fail()) {
        std::string line;
        getline(in, line);

        if (line.empty() || startsWithIgnoreToken(line)
            || (line.find_first_not_of(" \t\n\v\f\r") == std::string::npos)) {
            continue;
        }

        m_lines.push_back(line);

        std::string buf;
        std::stringstream ss(line);

        std::vector<std::string> line_tokens;
        while (ss >> buf) {
            line_tokens.push_back(buf);
        }
        m_tokens.push_back(line_tokens);
    };

    in.close();
    return true;
}

bool FileTokenizer::ReadFile(const char* filename, unsigned int firstLine, unsigned int maxNumLines)
{
    if (filename == nullptr) {
        return false;
    }

    std::ifstream in(filename);
    if (!in.is_open()) {
        fprintf(stderr, "Cannot open file '%s' for reading!\n", filename);
        return false;
    }

    ClearTokens();
    m_lines.clear();

    if (maxNumLines == 0) {
        maxNumLines = std::numeric_limits<unsigned int>::max();
    }

    unsigned int lineCounter = 0;
    while (!in.eof() && !in.fail() && lineCounter < maxNumLines) {
        std::string line;
        getline(in, line);

        if (lineCounter >= firstLine) {
            if (line.empty() || startsWithIgnoreToken(line)
                || (line.find_first_not_of(" \t\n\v\f\r") == std::string::npos)) {
                continue;
            }

            m_lines.push_back(line);

            std::string buf;
            std::stringstream ss(line);

            std::vector<std::string> line_tokens;
            while (ss >> buf) {
                line_tokens.push_back(buf);
            }
            m_tokens.push_back(line_tokens);
        }

        lineCounter++;
    };

    in.close();
    return true;
}

void FileTokenizer::ShowAll(FILE* fptr, bool prependLineNumber)
{
    fprintf(fptr, "\n");
    for (auto it = m_tokens.begin(); it != m_tokens.end(); it++) {
        if (prependLineNumber) {
            fprintf(fptr, "%4d : ", static_cast<int>(std::distance(m_tokens.begin(), it)) + 1);
        }
        for (size_t n = 0; n < it->size(); n++) {
            fprintf(fptr, "%s ", it->at(n).c_str());
        }
        fprintf(fptr, "\n");
    }
}

bool FileTokenizer::startsWithIgnoreToken(std::string& line)
{
    for (auto it : m_ignoreTokens) {
        if (line.compare(0, it.length(), it.data()) == 0) {
            return true;
        }
    }
    return false;
}

