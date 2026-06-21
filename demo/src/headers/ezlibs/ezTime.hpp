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

// ezTime is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include "ezOS.hpp"

#include <ctime>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <functional>

#ifndef EZ_TIME
#define EZ_TIME
#endif  // EZ_TIME

namespace ez {
namespace time {

// convert a string ISO8601 time to epoch time
inline bool iso8601ToEpoch(const std::string& vIsoDateTime, const std::string& vTimeFormat, std::time_t& vOutTime) {
    if (!vIsoDateTime.empty() && !vTimeFormat.empty()) {
        struct std::tm time = {};
        std::istringstream iss(vIsoDateTime);
        iss >> std::get_time(&time, vTimeFormat.c_str());
        if (!iss.fail()) {
            time.tm_hour = 0;
            time.tm_min = 0;
            time.tm_sec = 0;
#ifdef WINDOWS_OS
            vOutTime = _mkgmtime(&time);
#else
            vOutTime = timegm(&time);
#endif
            return true;
        }
    }
    return false;
}

// convert a epoch time to a string ISO8601 time
inline bool epochToISO8601(const std::time_t& vEpochTime, std::string& vOutTime) {
    auto tp = std::chrono::system_clock::from_time_t(vEpochTime);
    auto tt = std::chrono::system_clock::to_time_t(tp);
#ifdef _MSC_VER
    tm _timeinfo;
    tm* pTimeInfo = &_timeinfo;
    if (localtime_s(pTimeInfo, &tt) != 0) {
        return false;
    }
#else
    auto* pTimeInfo = std::localtime(&tt);
#endif
    std::ostringstream oss;
    oss << std::put_time(pTimeInfo, "%Y-%m-%d");
    if (!oss.fail()) {
        vOutTime = oss.str();
        return true;
    }
    return false;
}

// TODO: TO TEST
inline tm decomposeEpoch(const std::time_t& vEpochTime) {
    tm ret{};
    auto tp = std::chrono::system_clock::from_time_t(vEpochTime);
    auto tt = std::chrono::system_clock::to_time_t(tp);
#ifdef _MSC_VER
    tm _timeinfo;
    tm* pTimeInfo = &_timeinfo;
    if (localtime_s(pTimeInfo, &tt) != 0) {
        return ret;
    }
#else
    auto* pTimeInfo = std::localtime(&tt);
#endif
    ret = *pTimeInfo;
    return ret;
}

// TODO: TO TEST
inline std::string getCurrentDate() {
    auto curr_date_t = std::time(nullptr);
#ifdef _MSC_VER
    tm _timeinfo;
    tm* tm_curr_date = &_timeinfo;
    if (localtime_s(tm_curr_date, &curr_date_t) != 0) {
        return {};
    }
#else
    auto* tm_curr_date = std::localtime(&curr_date_t);
#endif
    char buffer[32 + 1]{};
    auto s = strftime(buffer, 32, "%Y-%m-%d", tm_curr_date);
    buffer[s] = '\0';
    return std::string(buffer);
}

// TODO: TO TEST
inline std::string getCurrentDate(size_t vHoursOffset) {
    auto curr_date_t = std::time(nullptr);
#ifdef _MSC_VER
    tm _timeinfo;
    tm* tm_curr_date = &_timeinfo;
    if (localtime_s(tm_curr_date, &curr_date_t) != 0) {
        return {};
    }
#else
    std::localtime(&curr_date_t);
#endif
    std::chrono::hours offset_hours(vHoursOffset);
    auto curr_date = std::chrono::system_clock::from_time_t(curr_date_t);
    auto offset_date_t = std::chrono::system_clock::to_time_t(curr_date + offset_hours);
#ifdef _MSC_VER
    tm* tm_offset_date = &_timeinfo;
    if (localtime_s(tm_offset_date, &offset_date_t) != 0) {
        return {};
    }
#else
    auto* tm_offset_date = std::localtime(&offset_date_t);
#endif
    char buffer[32+1]{};
    auto s = strftime(buffer, 32, "%Y-%m-%d", tm_offset_date);
    buffer[s] = '\0';
    return std::string(buffer);
}

inline uint64_t getTicks() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

/*
double time_measure{};
{
    ScopedTimer t(time_measure);
    // working
}
std::cout << "Measured time : " << time_measure << std::endl;
*/
class ScopedTimer {
    double& m_target;
    uint64_t m_start{};

public:
    ScopedTimer(double& target) : m_target(target), m_start(getTicks()) {}
    ~ScopedTimer() { m_target = static_cast<double>(getTicks() - m_start); }
};

// TODO: TO TEST
inline float getTimeInterval() {
    static auto S_lastTick = getTicks();
    const uint64_t ticks = getTicks();
    static float secMult = 1.0f / 1000.0f;
    const float interval = (ticks - S_lastTick) * secMult;
    S_lastTick = ticks;
    return interval;
}

// TODO: TO TEST
class ActionTime {
private:
    uint64_t m_LastTick = 0U;
    uint64_t m_PauseTick = 0U;
    uint64_t m_ResumeTick = 0U;
    bool m_Play = true;

public:
    ActionTime() {
        m_LastTick = getTicks();
    }
    
    void fix() {  // fixe les marqueur de temps
        m_LastTick = getTicks();
        m_PauseTick = getTicks();
        m_ResumeTick = getTicks();
    }
    
    void fixTime(double vValue) {  // fixe les marqueur de temps
        fix();
        setTime(vValue);
    }
    
    void pause() {
        m_PauseTick = getTicks();
        m_Play = false;
    }
    
    void resume() {
        m_ResumeTick = getTicks();
        m_LastTick += m_ResumeTick - m_PauseTick;
        m_Play = true;
    }
    
    uint64_t get() const {
        if (!m_Play) {
            return m_PauseTick - m_LastTick;
        }
        return getTicks() - m_LastTick;
    }
    
    double getTime() const {
        static double secMult = 1e-3;
        return get() * secMult;
    }

    void setTime(double vValue){  // set le temps
        const auto v = (uint64_t)(vValue * 1000.0);
        m_LastTick = getTicks() - v;
    }

    // verifie si vMs millisecond depuis le dernier fix et donc si on peut agir
    // vFix permet de fixer le temps pour la prochaine action
    // on pourrait vouloir interroger sans vouloir permettre la prochaine action
    bool isTimeToAct(long vMs, bool vFix) {
        if (get() > (uint64_t)vMs) {
            if (vFix) {
                fix();
            }
            return true;
        }
        return false;
    }
};

// TODO: TO TEST
//! Measure the time in microsecondes of a operation passed via lambda fucntion
//! the operation will be executed many time according to vCountOperations and averaged
template <typename TLAMBDA>
double measureOperationUs(TLAMBDA vLambda, size_t vCountOperations = 1) {
    using clock = std::chrono::high_resolution_clock;
    std::chrono::duration<double, std::micro> total{0};
    for (size_t i = 0; i < vCountOperations; ++i) {
        auto t0 = clock::now();
        vLambda();
        total += clock::now() - t0;
    }
    if (vCountOperations > 1) {
        total /= static_cast<double>(vCountOperations);
    }
    return total.count();
}

// will wait the condition to be true with a timeout and a step for inc time until ok or timeout
inline bool waitUntil(const std::function<bool(void)>& vCond, uint32_t vTimeoutMs, uint32_t vStepMs = 50) {
    using clock = std::chrono::steady_clock;
    auto start = clock::now();
    while (!vCond()) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count() >= vTimeoutMs) {
            return vCond();  // last chance
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(vStepMs));
    }
    return true;
}

}  // namespace time
}  // namespace ez
