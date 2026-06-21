#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezStr is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#ifndef EZ_STR
#define EZ_STR
#endif  // EZ_STR

#include <ios>
#include <set>
#include <list>
#include <cmath>
#include <vector>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cstdarg> // variadic
#include <clocale>  // std::setlocale
#include <locale>   // toupper, tolower (with locale)

#include "ezOS.hpp"

#ifdef WINDOWS_OS
#include <cwchar>
#include "Windows.h"
#endif


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) || defined(__MINGW32__)
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

namespace ez {
namespace str {

inline std::list<std::string> splitStringToList(const std::string& text, const std::string& delimiters, bool pushEmpty = false, bool vInversion = false) {
    std::list<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find_first_of(delimiters, start);
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty)) {
                if (vInversion)
                    arr.emplace_front(token);
                else
                    arr.emplace_back(token);
            }
            start = end + 1;
            end = text.find_first_of(delimiters, start);
        }
        std::string token = text.substr(start);
        if (!token.empty() || (token.empty() && pushEmpty)) {
            if (vInversion)
                arr.emplace_front(token);
            else
                arr.emplace_back(token);
        }
    }
    return arr;
}

inline std::vector<std::string> splitStringToVector(const std::string& text, const std::string& delimiters, bool pushEmpty = false) {
    std::vector<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find_first_of(delimiters, start);
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty))
                arr.emplace_back(token);
            start = end + 1;
            end = text.find_first_of(delimiters, start);
        }
        std::string token = text.substr(start);
        if (!token.empty() || (token.empty() && pushEmpty))
            arr.emplace_back(token);
    }
    return arr;
}

inline std::set<std::string> splitStringToSet(const std::string& text, const std::string& delimiters, bool pushEmpty = false) {
    std::set<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find_first_of(delimiters, start);
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty))
                arr.emplace(token);
            start = end + 1;
            end = text.find_first_of(delimiters, start);
        }
        std::string token = text.substr(start);
        if (!token.empty() || (token.empty() && pushEmpty))
            arr.emplace(token);
    }
    return arr;
}

inline std::list<std::string> splitStringToList(const std::string& text, char delimiter, bool pushEmpty = false, bool vInversion = false) {
    std::list<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find(delimiter, start);
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty)) {
                if (vInversion)
                    arr.emplace_front(token);
                else
                    arr.emplace_back(token);
            }
            start = end + 1;
            end = text.find(delimiter, start);
        }
        std::string token = text.substr(start);
        if (!token.empty() || (token.empty() && pushEmpty)) {
            if (vInversion)
                arr.emplace_front(token);
            else
                arr.emplace_back(token);
        }
    }
    return arr;
}

inline std::vector<std::string> splitStringToVector(const std::string& text, char delimiter, bool pushEmpty = false) {
    std::vector<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find(delimiter, start);
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty))
                arr.emplace_back(token);
            start = end + 1;
            end = text.find(delimiter, start);
        }
        std::string token = text.substr(start);
        if (!token.empty() || (token.empty() && pushEmpty))
            arr.emplace_back(token);
    }
    return arr;
}

inline std::set<std::string> splitStringToSet(const std::string& text, char delimiter, bool pushEmpty = false) {
    std::set<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find(delimiter, start);
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty))
                arr.emplace(token);
            start = end + 1;
            end = text.find(delimiter, start);
        }
        std::string token = text.substr(start);
        if (!token.empty() || (token.empty() && pushEmpty))
            arr.emplace(token);
    }
    return arr;
}

template <typename T>
inline bool stringToNumber(const std::string& vText, T& vNumber) {
    try {
        std::stringstream ss(vText);
        ss >> vNumber;
    } catch (std::exception&) { return false; }
    // if empty, must return false, elle true
    return !vText.empty();
}

template <typename T>
inline std::vector<T> stringToNumberVector(const std::string& text, char delimiter) {
    std::vector<T> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find(delimiter, start);
        T value = 0;
        while (end != std::string::npos) {
            std::string token = text.substr(start, end - start);
            if (stringToNumber<T>(token, value)) {
                arr.emplace_back(value);
            }
            start = end + 1;
            end = text.find(delimiter, start);
        }
        if (stringToNumber<T>(text.substr(start).c_str(), value)) {
            arr.emplace_back(value);
        }
    }
    return arr;
}

inline std::string toStr(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char TempBuffer[3072 + 1];  // 3072 = 1024 * 3
    const int w = vsnprintf(TempBuffer, 3072, fmt, args);
    va_end(args);
    if (w) {
        return std::string(TempBuffer, (size_t)w);
    }
    return std::string();
}

template< size_t MaxSize>
inline std::string toStr(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char TempBuffer[MaxSize+1];
    const int w = vsnprintf(TempBuffer, MaxSize, fmt, args);
    va_end(args);
    if (w) {
        return std::string(TempBuffer, (size_t)w);
    }
    return std::string();
}

inline std::string toUpper(const std::string& vStr, const std::locale& vLocale = {}) {
    std::string str = vStr;
    for (size_t i = 0U; i < str.size(); ++i) {
        str[i] = ::std::toupper(str[i], vLocale);
    }
    return str;
}

inline std::string toLower(const std::string& vStr, const std::locale& vLocale = {}) {
    std::string str = vStr;
    for (size_t i = 0U; i < str.size(); ++i) {
        str[i] = ::std::tolower(str[i], vLocale);
    }
    return str;
}

inline std::string toHex(const std::string& vStr) {
    if (!vStr.empty()) {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << std::hex;
        for (const auto& c : vStr) {
            ss << (0xff & (unsigned int)c);
        }
        return ss.str();
    }
    return {};
}

template <typename T>
inline std::string toStrFromArray(T* arr, size_t n, char delimiter = ';') {
    if (arr) {
        std::ostringstream os;
        for (size_t i = 0; i < n; ++i) {
            os << arr[i];
            if (i < n - 1)
                os << delimiter;
        }
        return os.str();
    }
    return "";
}

template <typename T>
inline std::string toStr(T t) {
    std::ostringstream os;
    os << t;
    return os.str();
}

template <typename T>
inline std::string toHexStr(T t) {
    std::ostringstream os;
    os << std::hex << t;
    return os.str();
}

template <typename T>
inline std::string toDecStr(T t) {
    std::ostringstream os;
    os << std::dec << t;
    return os.str();
}

inline std::vector<std::string::size_type> strContains(const std::string& text, const std::string& word, bool allowOverlap = false) {
    std::vector<std::string::size_type> result;
    if (!text.empty()) {
        std::string::size_type loc = 0;
        if (!word.empty()) {
            while ((loc = text.find(word, loc)) != std::string::npos) {
                result.emplace_back(loc);
                loc += allowOverlap ? 1 : word.size();
            }
        }
    }
    return result;
}

inline bool replaceString(std::string& str, const std::string& oldStr, const std::string& newStr) {
    bool found = false;
    if (!oldStr.empty()) {
        size_t pos = 0;
        while ((pos = str.find(oldStr, pos)) != std::string::npos) {
            found = true;
            str.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
    }
    return found;
}

inline size_t getCountOccurence(const std::string& vSrcString, const std::string& vStringToCount) {
    size_t count = 0;
    if (!vStringToCount.empty()) {
        size_t pos = 0;
        const auto len = vStringToCount.length();
        while ((pos = vSrcString.find(vStringToCount, pos)) != std::string::npos) {
            ++count;
            pos += len;
        }
    }
    return count;
}
inline size_t getCountOccurenceInSection(const std::string& vSrcString, size_t vStartPos, size_t vEndpos, const std::string& vStringToCount) {
    size_t count = 0;
    if (!vStringToCount.empty()) {
        size_t pos = vStartPos;
        const auto len = vStringToCount.length();
        while (pos < vEndpos && (pos = vSrcString.find(vStringToCount, pos)) != std::string::npos) {
            if (pos < vEndpos) {
                ++count;
                pos += len;
            }
        }
    }
    return count;
}

// can be more fast if char is used
inline size_t getCountOccurence(const std::string& vSrcString, const char& vStringToCount) {
    size_t count = 0;
    size_t pos = 0;
    while ((pos = vSrcString.find(vStringToCount, pos)) != std::string::npos) {
        ++count;
        pos++;
    }
    return count;
}

// can be more fast if char is used
inline size_t getCountOccurenceInSection(const std::string& vSrcString, size_t vStartPos, size_t vEndpos, const char& vStringToCount) {
    size_t count = 0;
    size_t pos = vStartPos;
    while (pos < vEndpos && (pos = vSrcString.find(vStringToCount, pos)) != std::string::npos) {
        if (pos < vEndpos) {
            ++count;
            pos++;
        }
    }
    return count;
}

// std::wstring to std::string
// std::wstring(unicode/multibytes/char16/wchar_t) to std::string(char)
inline std::string wstringToString(const std::wstring& wstr) {
    std::mbstate_t state = std::mbstate_t();
    const size_t len = wstr.size();
    std::vector<char> mbstr(len);
    const wchar_t* wptr = wstr.c_str();
#ifdef _MSC_VER
    size_t res = 0;
    /*errno_t err = */ wcsrtombs_s(&res, &mbstr[0], len, &wptr, mbstr.size(), &state);
#else
    std::wcsrtombs(&mbstr[0], &wptr, mbstr.size(), &state);
#endif
    return std::string(mbstr.data(), mbstr.size());
}

// std::string to std::wstring
// std::string(char) to std::wstring(unicode/multibytes/char16/wchar_t)
inline std::wstring stringToWstring(const std::string& mbstr) {
    std::mbstate_t state = std::mbstate_t();
    const size_t len = mbstr.size();
    std::vector<wchar_t> wstr(len);
    const char* ptr = mbstr.c_str();
#ifdef _MSC_VER
    size_t res = 0;
    /*errno_t err = */ mbsrtowcs_s(&res, &wstr[0], len, &ptr, wstr.size(), &state);
#else
    std::mbsrtowcs(&wstr[0], &ptr, wstr.size(), &state);
#endif
    return std::wstring(wstr.data(), wstr.size());
}

inline size_t getDigitsCountOfAIntegralNumber(const int64_t vNum) {
    size_t res = (vNum == 0) ? 1 : static_cast<int>(log10(std::abs(vNum))) + 1;
    if (vNum < 0) {
        ++res;
    }
    return res;
}

inline std::string utf8Encode(const std::wstring& wstr) {
    std::string res;
#ifdef WINDOWS_OS
    if (!wstr.empty()) {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        if (size_needed) {
            res = std::string(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &res[0], size_needed, NULL, NULL);
        }
    }
#else  // WINDOWS_OS
    // Suppress warnings from the compiler.
    (void)wstr;
#endif  // WINDOWS_OS
    return res;
}

// Convert an UTF8 string to a wide Unicode String
inline std::wstring utf8Decode(const std::string& str) {
    std::wstring res;
#ifdef WINDOWS_OS
    if (!str.empty()) {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        if (size_needed) {
            res = std::wstring(size_needed, 0);
            MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &res[0], size_needed);
        }
    }
#else  // WINDOWS_OS
    (void)str;
#endif  // WINDOWS_OS
    return res;
}

inline std::string searchForPatternWithWildcards(const std::string& vBuffer, const std::string& vWildcardedPattern, std::pair<size_t, size_t>& vOutPosRange) {
    std::string res;
    bool startsWithWildcard = !vWildcardedPattern.empty() && vWildcardedPattern.front() == '*';
    bool endsWithWildcard = !vWildcardedPattern.empty() && vWildcardedPattern.back() == '*';
    auto patterns = splitStringToVector(vWildcardedPattern, '*', false);
    vOutPosRange.first = std::string::npos;
    vOutPosRange.second = 0U;
    for (const std::string& pattern : patterns) {
        auto start = vBuffer.find(pattern, vOutPosRange.second);
        if (start != std::string::npos) {
            if (vOutPosRange.first == std::string::npos) {
                vOutPosRange.first = start;
            }
            vOutPosRange.second = start + pattern.size();
        } else {
            vOutPosRange.first = std::string::npos;
            vOutPosRange.second = std::string::npos;
            break;
        }
    }
    if (vOutPosRange.first != std::string::npos && vOutPosRange.second != std::string::npos) {
        if (startsWithWildcard) {
            vOutPosRange.first = 0U;
        }
        if (endsWithWildcard) {
            vOutPosRange.second = vBuffer.size();
        }
        res = vBuffer.substr(vOutPosRange.first, vOutPosRange.second - vOutPosRange.first);
    }
    return res;
}

inline std::string searchForPatternWithWildcards(const std::string& vBuffer, const std::string& vWildcardedPattern) {
    std::pair<size_t, size_t> posRange;
    return searchForPatternWithWildcards(vBuffer, vWildcardedPattern, posRange);
}

inline std::string encodeBase64(const std::string& in) {
    static constexpr char sEncodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (uint8_t c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(sEncodingTable[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back(sEncodingTable[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4) {
        out.push_back('=');
    }
    return out;
}

// todo: to test
inline std::vector<std::string> extractWildcardsFromPattern(const std::string& vBuffer, const std::string& vPattern) {
    std::vector<std::string> res;
    auto patterns = splitStringToVector(vPattern, '*', false);
    std::pair<size_t, size_t> range;
    range.first = std::string::npos;
    range.second = 0U;
    for (const std::string& pattern : patterns) {
        auto start = vBuffer.find(pattern, range.second);
        if (start != std::string::npos) {
            if (range.first != std::string::npos) {
                range.second = start;
                res.push_back(vBuffer.substr(range.first, range.second - range.first));
            }
            range.first = start + pattern.size();
        } else {
            range.first = std::string::npos;
            range.second = std::string::npos;
            break;
        }
    }
    return res;
}

inline bool endsWith(const std::string& vStr, const std::string& vSuffix) {
    if (vSuffix.size() > vStr.size()) {
        return false;
    }
    return (vStr.compare(vStr.size() - vSuffix.size(), vSuffix.size(), vSuffix) == 0);
}

}  // namespace str
}  // namespace ez

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
