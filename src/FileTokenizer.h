/**
 * File:    FileTokenizer.h
 * Author:  Thomas Mueller, HdA/MPIA
 *
 *  This file is part of GRPolygonRender.
 */
#ifndef GRPR_FILE_TOKENIZER_H
#define GRPR_FILE_TOKENIZER_H

#include <iostream>
#include <vector>

class FileTokenizer
{
public:
    FileTokenizer();
    explicit FileTokenizer(const char* filename);
    FileTokenizer(const char* filename, const char* ignToken);
    FileTokenizer(const char* filename, char ignToken);

    virtual ~FileTokenizer();

    /// Remove all ignore tokens from the ignore tokens list.
    void ClearIgnoreTokens();

    /// Remove all tokens read from file.
    void ClearTokens();

    int FindTokenInLine(const int lineNumber, const char* ftoken);

    const char* GetLine(const unsigned int lineNumber);

    /// Get number of lines read from the file.
    virtual unsigned int GetNumLines();

    /// Get number of sub tokens in line number 'lineNumber'.
    int GetNumSubTokens(const int lineNumber);

    /**
     * @brief Get sub token for base token 'btoken'.
     *
     * @param btoken  Base token (first token in row).
     * @param colNumber  Column number; note that colNumber=0 yields the base token itself, whereas colNumber=-1 yields
     *   the last columne value.
     * @param stok  Reference to the char token in column 'colNumber'. Note that you have
     *       to delete the memory of stok yourself.
     */
    bool GetSubToken(const char* btoken, int colNumber, char*& stok);

    bool GetSubBoolToken(const char* btoken, int colNumber, bool& bval);

    /**
     * @brief Get sub token for base token 'btoken' as integer value.
     *
     * @param btoken  Base token (first token in row).
     * @param colNumber  Column number; note that colNumber=0 yields the base token itself.
     * @param val  Reference to the integer token in column 'colNumber'.
     */
    template <typename T> bool GetSubToken(const char* btoken, int colNumber, T& val);

    template <typename T> bool GetSubTokens(const char* btoken, const int startCol, const int numCols, T* vals);

    /**
     * @brief Get sub token of valid line number 'lineNumber' in column 'colNumber'
     *   of type character.
     *
     * @param lineNumber
     * @param colNumber
     * @param stok    The ref has to be initialized (nullptr or allocated memory). Any previous
     *                value will be deleted.
     * @return
     */
    bool GetSubToken(const int lineNumber, int colNumber, char*& stok);

    template <typename T> bool GetSubToken(const int lineNumber, int colNumber, T& val);

    /**
     * @brief Get sub tokens.
     *   Note that 'vals' has to be allocated outside with minimum size of 'numCols'.
     * @param lineNumber
     * @param numCols
     * @param vals
     */
    template <typename T> bool GetSubTokens(const int lineNumber, const int numCols, T* vals);

    /**
     * @brief Get 'numCols' sub tokens starting from column 'startCol'.
     *   Note that column number '0' usually represents the parameter name.
     *
     * @param lineNumber
     * @param startCol      First column number where the parameters are stored.
     * @param numCols       Number of columns to be read.
     * @param vals          Pointer to data - note that the vals has to be of the correct size
     *                        (len(T) >= numCols).
     */
    template <typename T> bool GetSubTokens(const int lineNumber, const int startCol, const int numCols, T* vals);

    /// Insert ignore token
    void InsertIgnoreToken(const char itoken);

    /// Insert ignore token.
    void InsertIgnoreToken(const char* itoken);

    /**
     * @brief Check whether 'token' is a base token (first token in row).
     *
     * @param token     Token name to be tested if it exists as base token.
     * @param lineNum   Reference to line number where the token was found.
     * @param numCols   Reference to the number of columns in the row where the token was found.
     * @return true if 'token' is a base token.
     */
    bool IsBaseToken(const char* token, int& lineNum, int& numCols);

    bool IsBaseTokenValid(const char* token, unsigned int lineNumber);

    bool IsBaseTokenValid(const char* token, unsigned int lineNumber, int& numCols);

    void PrintLine(const int lineNumber, FILE* fptr = stdout);

    /**
     * @brief  Read file to be tokenized.
     *
     *  You have to call this method yourself if you have used the standard c'tor.
     * @param filename  File name.
     * @return true if file exists
     */
    virtual bool ReadFile(const char* filename);

    virtual bool ReadFile(const char* filename, unsigned int firstLine, unsigned int maxNumLines = 1);

    /// Show all tokens read from file.
    void ShowAll(FILE* fptr = stdout, bool prependLineNumber = false);

protected:
    /**
     * @brief Test if line number and column number are valid.
     *    If column number is negative, then the column number will count from last column.
     *    For example, colNumber = -1 will be set to first element = 0.
     * @param lineNumber
     * @param colNumber    Reference to column number. Note that this value will be modified if negative.
     * @return
     */
    bool isValid(const int lineNumber, int& colNumber);

    bool isValid(const int lineNumber);

    /// Test if string 'line' starts with an ignore token.
    bool startsWithIgnoreToken(std::string& line);

    /// Tokenize file.
    bool tokenize();

protected:
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

    //! @brief List of ignore tokens.
    //! A line that starts with one of these tokens will be ignored.
    std::vector<std::string> m_ignoreTokens;

    std::string m_filename; //!<  Name of file from which tokens were read.
    std::vector<std::string> m_lines; //!< List of all lines stored as strings.
    std::vector<std::vector<std::string>> m_tokens; //!< List of all tokens stored by lines.
#ifdef _WIN32
#pragma warning(pop)
#endif
};

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4244) // type conversion warning
#endif
template <typename T> bool FileTokenizer::GetSubToken(const char* btoken, int colNumber, T& val)
{
    int lineNum, numCols;
    if (!IsBaseToken(btoken, lineNum, numCols)) {
        return false;
    }

    if (colNumber < numCols) {
        return this->GetSubToken<T>(lineNum, colNumber, val);
    }
    return false;
}

template <typename T>
bool FileTokenizer::GetSubTokens(const char* btoken, const int startCol, const int numCols, T* vals)
{
    int lineNum, numAvailCols;
    if (!IsBaseToken(btoken, lineNum, numAvailCols)) {
        return false;
    }

    bool isOkay = true;
    int n = startCol;
    while (isOkay && n < numAvailCols && (n - startCol) < numCols) {
        T sval;
        isOkay &= GetSubToken<T>(lineNum, n, sval);
        if (isOkay) {
            vals[n - startCol] = sval;
        }
        n++;
    }
    return isOkay;
}

template <typename T> bool FileTokenizer::GetSubToken(const int lineNumber, int colNumber, T& val)
{
    if (!isValid(lineNumber, colNumber)) {
        return false;
    }

    // get integer
    if (std::is_same<T, int>::value || std::is_same<T, short>::value) {
        val = static_cast<T>(std::atoi(m_tokens[lineNumber][colNumber].c_str()));
    }
    // get unsigned integer
    else if (std::is_same<T, unsigned int>::value || std::is_same<T, long>::value) {
        char* end;
        val = static_cast<T>(std::strtoul(m_tokens[lineNumber][colNumber].c_str(), &end, 0));
    }
    // get float or double
    else if (std::is_same<T, float>::value || std::is_same<T, double>::value) {
        val = static_cast<T>(std::atof(m_tokens[lineNumber][colNumber].c_str()));
    }
    else {
        fprintf(stderr, "FileTokenizer::GetSubToken(const int,int,T) ... Parameter type 'T' not allowed!\n");
        return false;
    }
    return true;
}

/**
 *
 */
template <typename T> bool FileTokenizer::GetSubTokens(const int lineNumber, const int numCols, T* vals)
{
    return GetSubTokens<T>(lineNumber, 1, numCols, vals);
}

/**
 *
 */
template <typename T>
bool FileTokenizer::GetSubTokens(const int lineNumber, const int startCol, const int numCols, T* vals)
{
    bool isOkay = true;
    int i = 0;
    while (i < numCols && isOkay) {
        T value;
        isOkay &= GetSubToken<T>(lineNumber, startCol + i, value);
        if (isOkay) {
            vals[i] = value;
        }
        i++;
    }
    return isOkay;
}

#endif // GRPR_FILE_TOKENIZER_H
