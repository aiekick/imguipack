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

// ezApp is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include "ezOS.hpp"

#ifdef WINDOWS_OS
// includes
#include <Windows.h>
#include <shellapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <direct.h>  // _chdir
// defines
#define S_IFDIR _S_IFDIR
#define GetCurrentDir _getcwd
#define SetCurrentDir _chdir
#define SLASH_CHAR "\\"
#elif defined(LINUX_OS) || defined(APPLE_OS)
// includes
#include <unistd.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <ctype.h>
#include <pwd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstdlib>
#include <stdio.h>
#ifdef APPLE_OS
#include <dlfcn.h>
#include <sys/syslimits.h>  // PATH_MAX
#endif                      // APPLE_OS
#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#else  // STDC_HEADERS
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif  // HAVE_STDLIB_H
#endif  /* STDC_HEADERS */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
// defines
#define GetCurrentDir getcwd
#define SetCurrentDir chdir
#ifndef S_IFDIR
#define S_IFDIR __S_IFDIR
#endif  // S_IFDIR
#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif  // MAX_PATH
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif  // PATH_MAX
#define SLASH_CHAR "/"
#endif

#include "ezStr.hpp"  // ez::str::replaceString

#include <map>
#include <vector>
#include <string>
#include <cstdio>   // FILENAME_MAX
#include <cstdint>  // int32_t
#include <iostream>

namespace ez {

class App {
private:
    std::string m_AppPath;

public:
    App() = default;
    App(int32_t /*vArgc*/, char** vArgv, bool vSetCurrentDir = true) {
#ifdef _MSC_VER
        SetConsoleOutputCP(CP_UTF8);
#endif
        setAppPath(vArgv[0]);
        if (vSetCurrentDir) {
            setCurDirectory(getAppPath());
        }
    }

    void setAppPath(const std::string& vPath) {
        if (!vPath.empty()) {
            auto pos = vPath.find_last_of("\\/");
            if (pos != std::string::npos) {
                m_AppPath = vPath.substr(0, pos);
            }
        }
    }

    std::string getAppPath() {
        if (m_AppPath.empty()) {
            char buffer[MAX_PATH] = {};
#ifdef WINDOWS_OS
            GetModuleFileName(nullptr, buffer, MAX_PATH);
#elif defined(LINUX_OS)
            char szTmp[32];
            sprintf(szTmp, "/proc/%d/exe", getpid());
            const int max_len = MAX_PATH - 1;
            int bytes = readlink(szTmp, buffer, MAX_PATH);
            if (bytes > max_len) { bytes = max_len; }
            if (bytes >= 0) { buffer[bytes] = '\0'; }
#elif defined(APPLE_OS)
            auto path = m_getMacOsAppPath();
            auto pos = path.find_last_of("\\/");
            m_AppPath = path.substr(0, pos);
#endif
#if defined(WINDOWS_OS) || defined(LINUX_OS)
            auto pos = std::string(buffer).find_last_of("\\/");
            m_AppPath = std::string(buffer).substr(0, pos);
#endif
        }
        return m_AppPath;
    }

    std::string getAppPath() const { return m_AppPath; }

    static std::string getCurDirectory() {
        char cCurrentPath[FILENAME_MAX];
        if (GetCurrentDir(cCurrentPath, FILENAME_MAX)) {
            return std::string(cCurrentPath) + SLASH_CHAR;
        }
        return "";
    }

    static bool setCurDirectory(const std::string& vPath) {
        auto path = vPath;
        m_correctSlashForPath(path);
        return (SetCurrentDir(path.c_str()) == 0);
    }

private:
    /* correct file path between os and different slash type between window and unix */
    static void m_correctSlashForPath(std::string& vPath) {
        str::replaceString(vPath, "\\", SLASH_CHAR);
        str::replaceString(vPath, "/", SLASH_CHAR);
    }

#if defined(APPLE_OS)
    static std::string m_getMacOsAppPath() {
        Dl_info module_info;
        if (dladdr((void*)getCurDirectory, &module_info)) {  // use of getCurDirectory who is a static function
            return std::string(module_info.dli_fname);
        }
        return "";
    }
#endif
};

}  // namespace ez
