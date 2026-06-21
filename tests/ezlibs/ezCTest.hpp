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

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Set of CTest macros

#include "ezOS.hpp"
#include "ezLog.hpp"

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

#define CTEST_ASSERT(cond) \
    if (!(cond))           \
    return false

#define CTEST_ASSERT_MESSAGE(cond, str)                    \
    if (!(cond)) {                                         \
        LogVarLightError("Test : Error => (%s) is false", #cond);  \
        return false;                                      \
    } else {                                               \
        LogVarLightError("Test : Succeed => (%s) is true", #cond); \
    }

#define CTEST_ASSERT_MESSAGE_DELAYED(status, cond, str)    \
    if (!(cond)) {                                         \
        LogVarLightError("Test : Error => (%s) is false", #cond);  \
        status = false;                                    \
    } else {                                               \
        LogVarLightError("Test : Succeed => (%s) is true", #cond); \
    }

#define CTEST_TRY_CATCH(EXPR)         \
    {                                 \
        bool exceptionThrown = false; \
        try {                         \
            (EXPR);                   \
        } catch (...) {               \
            exceptionThrown = true;   \
        }                             \
        if (!exceptionThrown) {       \
            return false;             \
        }                             \
    }

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

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
