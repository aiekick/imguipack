/*
MIT License

Copyright (c) 2010-2020 Stephane Cuillerdier (aka Aiekick)

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

// ezLog is part of the ezLibs project : https://github.com/aiekick/ezLibs.git
// ezLog is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#pragma once
#pragma warning(disable : 4251)

#ifndef EZ_TOOLS_LOG
#define EZ_TOOLS_LOG
#endif  // EZ_TOOLS_LOG

#ifndef EZ_LOG_APP_NAME
#define EZ_LOG_APP_NAME "App"
#endif

#include "ezStr.hpp"
#include "ezTime.hpp"

/* WARNING
* FOR USING THE OLD BEAHVIOR SINGLETON
* YOU MUST DO : #define LEGACY_SINGLETON
*/

#ifdef USE_GLFW3
#include <GLFW/glfw3.h>
#elif defined(USE_SDL2)
#include <SDL.h>
#endif
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
#include <tracy/Tracy.hpp>
#endif

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include <cstdarg> /* va_list, va_start, va_arg, va_end */

#include <iostream>  // std::cout

#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <fstream>
#include <stdexcept>
#include <functional>

typedef long long int64;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifdef LEGACY_SINGLETON
#define IsVerboseMode (ez::Log::ref().isVerboseMode() == true)

#define LogVarError(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarWarning(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarInfo(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarDebugError(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction_Debug(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarDebugWarning(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction_Debug(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarDebugInfo(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction_Debug(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarLightError(s, ...) ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, s, ##__VA_ARGS__)

#define LogVarLightWarning(s, ...) ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, s, ##__VA_ARGS__)

#define LogVarLightInfo(s, ...) ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, s, ##__VA_ARGS__)

#define LogVarTag(t, s, ...) ez::Log::ref().logStringByTypeWithFunction(t, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarLightTag(t, s, ...) ez::Log::ref().logSimpleStringByType(t, s, ##__VA_ARGS__)

#define LogAssert(a, b, ...)               \
    if (!(a)) {                            \
        LogVarDebugInfo(b, ##__VA_ARGS__); \
        assert(a);                         \
    }

#ifdef USE_OPENGL
#define LogGlError() ez::Log::ref().logGLError("" /*__FILE__*/, __FUNCTION__, __LINE__, "")
#define LogGlErrorVar(var) ez::Log::ref().logGLError("" /*__FILE__*/, __FUNCTION__, __LINE__, var)
#endif
#else  // LEGACY_SINGLETON#define IsVerboseMode (ez::Log::ref().isVerboseMode() == true)

#define LogVarError(s, ...) ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarWarning(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarInfo(s, ...) ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarDebugError(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction_Debug(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarDebugWarning(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction_Debug(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarDebugInfo(s, ...) \
    ez::Log::ref().logStringByTypeWithFunction_Debug(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarLightError(s, ...) ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, s, ##__VA_ARGS__)

#define LogVarLightWarning(s, ...) ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, s, ##__VA_ARGS__)

#define LogVarLightInfo(s, ...) ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, s, ##__VA_ARGS__)

#define LogVarTag(t, s, ...) ez::Log::ref().logStringByTypeWithFunction(t, std::string(__FUNCTION__), (int)(__LINE__), s, ##__VA_ARGS__)

#define LogVarLightTag(t, s, ...) ez::Log::ref().logSimpleStringByType(t, s, ##__VA_ARGS__)

#define LogAssert(a, b, ...)               \
    if (!(a)) {                            \
        LogVarDebugInfo(b, ##__VA_ARGS__); \
        assert(a);                         \
    }

#ifdef USE_OPENGL
#define LogGlError() ez::Log::ref().logGLError("" /*__FILE__*/, __FUNCTION__, __LINE__, "")
#define LogGlErrorVar(var) ez::Log::ref().logGLError("" /*__FILE__*/, __FUNCTION__, __LINE__, var)
#endif
#endif  // LEGACY_SINGLETON
namespace ez {

class Log {
public:
    typedef int MessageType;
    typedef std::function<void(const int& vType, const std::string& vMessage)> LogMessageFunctor;
    enum MessageTypeEnum { LOGGING_MESSAGE_TYPE_INFOS = 0, LOGGING_MESSAGE_TYPE_WARNING, LOGGING_MESSAGE_TYPE_ERROR };

protected:
    std::mutex m_logger_Mutex;

private:
    static size_t constexpr sMAX_BUFFER_SIZE = 1024U * 3U;
    std::ofstream m_debugLogFile;
    int64 m_lastTick = 0;
    bool m_reseted = false;
    LogMessageFunctor m_standardLogFunction;
    LogMessageFunctor m_openGLLogFunction;
    std::vector<std::string> m_messages;  // file, function, line, msg
    bool m_consoleVerbose = false;

public:
    Log() {
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        m_createFileOnDisk();
    }
    ~Log() {
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        close();
    }
    void logSimpleString(const char* fmt, ...) {
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        va_list args;
        va_start(args, fmt);
        m_LogString(nullptr, nullptr, nullptr, fmt, args);
        va_end(args);
        lck.unlock();
    }
    void logSimpleStringByType(const MessageType& vType, const char* fmt, ...) {
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        va_list args;
        va_start(args, fmt);
        m_LogString(&vType, nullptr, nullptr, fmt, args);
        va_end(args);
        lck.unlock();
    }
    void logStringWithFunction(const std::string& vFunction, const int& vLine, const char* fmt, ...) {
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        va_list args;
        va_start(args, fmt);
        m_LogString(nullptr, &vFunction, &vLine, fmt, args);
        va_end(args);
        lck.unlock();
    }
    void logStringByTypeWithFunction(const MessageType& vType, const std::string& vFunction, const int& vLine, const char* fmt, ...) {
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        va_list args;
        va_start(args, fmt);
        m_LogString(&vType, &vFunction, &vLine, fmt, args);
        va_end(args);
        lck.unlock();
    }
    void logStringByTypeWithFunction_Debug(const MessageType& vType, const std::string& vFunction, const int& vLine, const char* fmt, ...) {
#ifndef NDEBUG
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        va_list args;
        va_start(args, fmt);
        m_LogString(&vType, &vFunction, &vLine, fmt, args);
        va_end(args);
        lck.unlock();
#else
        (void)vType;
        (void)vFunction;
        (void)vLine;
        (void)fmt;
#endif
    }
    void logStringWithFunction_Debug(const std::string& vFunction, const int& vLine, const char* fmt, ...) {
#ifndef NDEBUG
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        va_list args;
        va_start(args, fmt);
        m_LogString(nullptr, &vFunction, &vLine, fmt, args);
        va_end(args);
        lck.unlock();
#else
        (void)vFunction;
        (void)vLine;
        (void)fmt;
#endif
    }
#ifdef USE_OPENGL
    bool logGLError(const std::string& vFile, const std::string& vFunc, int vLine, const std::string& vGLFunc = "") const {
        (void)vFile;

#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        if (!m_consoleVerbose) {
            return false;
        }

        const GLenum err(glGetError());
        if (err != GL_NO_ERROR) {
            std::string error;

            switch (err) {
                case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
                case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
                case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
                //case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
                case GL_STACK_UNDERFLOW: error = "GL_STACK_UNDERFLOW"; break;
                case GL_STACK_OVERFLOW: error = "GL_STACK_OVERFLOW"; break;
            }

            if (!error.empty()) {
                const auto ticks = ez::time::getTicks();
                const auto time = (ticks - m_lastTick) / 1000.0f;

                std::string msg;

                if (!vGLFunc.empty()) {
#ifdef USE_GLFW3
                    msg = str::toStr(
                        "[%010.3fs][GLFW3 0x%X][%s:%i] %s in %s\n", time, (uintptr_t)glfwGetCurrentContext(), vFunc.c_str(), vLine, error.c_str(), vGLFunc.c_str());
#elif defined(USE_SDL2)
                    msg = str::toStr(
                        "[%010.3fs][SDL2 0x%X][%s:%i] %s in %s\n", time, (uintptr_t)SDL_GL_GetCurrentContext(), vFunc.c_str(), vLine, error.c_str(), vGLFunc.c_str());
#endif
                } else {
#ifdef USE_GLFW3
                    msg = str::toStr("[%010.3fs][GLFW3 0x%X][%s:%i] %s\n", time, (uintptr_t)glfwGetCurrentContext(), vFunc.c_str(), vLine, error.c_str());
#elif defined(USE_SDL2)
                    msg = str::toStr("[%010.3fs][SDL2 0x%X][%s:%i] %s\n", time, (uintptr_t)SDL_GL_GetCurrentContext(), vFunc.c_str(), vLine, error.c_str());
#endif
                }

                LogVarLightError("%s", msg.c_str());

                if (m_openGLLogFunction != nullptr) {
                    auto arr = str::splitStringToVector(msg, '\n');
                    if (arr.size() == 1U) {
                        m_openGLLogFunction(2, msg);
                    } else {
                        for (auto m : arr) {
                            m_openGLLogFunction(2, m);
                        }
                    }
                }

                if (!m_debugLogFile.bad()) {
                    m_debugLogFile << msg << std::endl;
                }

                return true;
            }
        }

        return false;
    }
#endif
    void close() {
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        m_debugLogFile.close();
        m_debugLogFile.clear();
        lck.unlock();
    }

    std::string getLastErrorAsString() {
        std::string msg;

#ifdef _MSC_VER
        // Get the error message, if any.
        const DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0 || errorMessageID == 6) {
            return std::string();  // No error message has been recorded
        }

        LPSTR messageBuffer = nullptr;
        const size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errorMessageID,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer,
            0,
            nullptr);

        msg = std::string(messageBuffer, size);

        // Free the buffer.
        LocalFree(messageBuffer);
#else
        // cAssert(0, "to implement");
#endif
        return msg;
    }

    void setStandardLogMessageFunctor(const LogMessageFunctor& vMessageLogFunctor) { m_standardLogFunction = vMessageLogFunctor; }
    void setOpenglLogMessageFunctor(const LogMessageFunctor& vMessageLogFunctor) { m_openGLLogFunction = vMessageLogFunctor; }

    void setVerboseMode(bool vFlag) { m_consoleVerbose = vFlag; }
    bool isVerboseMode() { return m_consoleVerbose; }

private:
    void m_createFileOnDisk() {
        if (m_reseted) {
            return;
        }
#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
        ZoneScoped;
#endif
        std::unique_lock<std::mutex> lck(m_logger_Mutex, std::defer_lock);
        lck.lock();
        m_debugLogFile.open("debug.log", std::ios::out);
        m_lastTick = time::getTicks();
        m_consoleVerbose = false;
        m_reseted = true;
        lck.unlock();
    }

private:
    void m_LogString(const MessageType* vType, const std::string* vFunction, const int* vLine, const char* vStr) {
        const int64 ticks = time::getTicks();
        const double time = (ticks - m_lastTick) / 1000.0;

        static char TempBufferBis[sMAX_BUFFER_SIZE + 1];
        int w = 0;
        if (vFunction && vLine) {
            w = snprintf(TempBufferBis, sMAX_BUFFER_SIZE, "[%010.3fs][%s:%i] %s", time, vFunction->c_str(), *vLine, vStr);
        } else {
            w = snprintf(TempBufferBis, sMAX_BUFFER_SIZE, "[%010.3fs] %s", time, vStr);
        }
        if (w) {
            const std::string msg = std::string(TempBufferBis, (size_t)w);

            m_messages.push_back(msg);

#if defined(TRACY_ENABLE) && defined(LOG_TRACY_MESSAGES)
            TracyMessageL(m_messages[m_messages.size() - 1U].c_str());
#endif

#ifdef __ANDROID__
            if (*vType == MessageTypeEnum::LOGGING_MESSAGE_TYPE_INFOS) {
                __android_log_write(ANDROID_LOG_INFO, EZ_LOG_APP_NAME, msg.c_str());
            } else if (*vType == MessageTypeEnum::LOGGING_MESSAGE_TYPE_WARNING) {
                __android_log_write(ANDROID_LOG_WARN, EZ_LOG_APP_NAME, msg.c_str());
            } else if (*vType == MessageTypeEnum::LOGGING_MESSAGE_TYPE_ERROR) {
                __android_log_write(ANDROID_LOG_ERROR, EZ_LOG_APP_NAME, msg.c_str());
            }
#else
            std::cout << msg << std::endl;
#endif

            if (vStr && m_standardLogFunction) {
                int type = 0;

                if (vType) {
                    type = (int)(*vType);
                }

                auto arr = str::splitStringToVector(msg, '\n');
                if (arr.size() == 1U) {
                    m_standardLogFunction(type, msg);
                } else {
                    for (auto m : arr) {
                        m_standardLogFunction(type, m);
                    }
                }
            }

            if (!m_debugLogFile.bad()) { m_debugLogFile << msg << "\n"; }
        }
    }
    void m_LogString(const MessageType* vType, const std::string* vFunction, const int* vLine, const char* fmt, va_list vArgs) {
        static char TempBuffer[sMAX_BUFFER_SIZE + 1];
        int w = vsnprintf(TempBuffer, sMAX_BUFFER_SIZE, fmt, vArgs);
        if (w) {
            m_LogString(vType, vFunction, vLine, TempBuffer);
        }
    }

public:
// old behavior
#ifdef LEGACY_SINGLETON
    static Log* Instance(Log* vCopyPtr = nullptr, bool vForce = false) {
        static auto instance_ptr = std::unique_ptr<Log>(new Log());  // std::make_unique is not available in cpp11
        static ez::Log* _instance_copy = nullptr;
        if (vCopyPtr || vForce) {
            _instance_copy = vCopyPtr;
        } else if (_instance_copy == nullptr) {
            instance_ptr->m_createFileOnDisk();
        }
        if (_instance_copy) {
            return _instance_copy;
        }
        return instance_ptr.get();
    }
#else  // LEGACY_SINGLETON
private:
    static std::unique_ptr<Log>& getSingletonPtr() {
        static std::unique_ptr<Log> mp_instance;
        return mp_instance;
    }
    // borrowed pointer to an externally-owned Log (host instance, when called
    // from a DLL). raw on purpose — we MUST NOT delete it on shutdown.
    static Log*& getBorrowedPtr() {
        static Log* mp_borrowed = nullptr;
        return mp_borrowed;
    }

public:
    // initSingleton(nullptr) → create and own a fresh instance (default)
    // initSingleton(hostPtr) → reuse the host's instance without taking ownership.
    //   useful when the consumer is a plugin DLL that wants all its logs to
    //   land in the host's standardLogFunctor (and thus the host's console pane)
    static void initSingleton(Log* vBorrowedHostInstance = nullptr) {
        if (vBorrowedHostInstance != nullptr) {
            getBorrowedPtr() = vBorrowedHostInstance;
            getSingletonPtr().reset();  // make sure we don't keep a stale owned instance
        } else {
            getBorrowedPtr() = nullptr;
            getSingletonPtr().reset(new Log());
        }
    }
    static Log& ref() {
        if (auto* borrowed = getBorrowedPtr()) {
            return *borrowed;
        }
        return *getSingletonPtr().get();
    }
    static void unitSingleton() {
        getBorrowedPtr() = nullptr;   // release the borrow, never delete
        getSingletonPtr().reset();    // destroy our own owned instance (if any)
    }

#endif  // LEGACY_SINGLETON
};

}  // namespace ez
