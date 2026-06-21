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

// ezSingleton is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

/* a singleton design who does not cause memory leak in your leak checker

macro to add just after your class definition.

==========================================================================
USAGE 1 - type with a default constructor (no argument needed)
==========================================================================

class Foo {
    IMPLEMENT_SINGLETON(Foo)
public:
    Foo() = default;
    void doStuff();
};

// at program start
Foo::initSingleton();

// anywhere
Foo::ref().doStuff();

// at program end
Foo::unitSingleton();

==========================================================================
USAGE 2 - type whose constructor needs argument(s)
==========================================================================

class Bar {
    IMPLEMENT_SINGLETON(Bar)
public:
    explicit Bar(IApp& aApp);  // no default ctor required
    void doStuff();
private:
    IApp& mr_app;
};

// at program start - pass the arguments forwarded to Bar's constructor
Bar::initSingleton(myApp);

// anywhere - Bar::ref() does NOT need the arguments anymore
Bar::ref().doStuff();

// at program end
Bar::unitSingleton();

==========================================================================
KEY POINTS
==========================================================================

 - initSingleton(args...) builds the instance ONLY on the first call,
   when the internal pointer is null. Subsequent calls IGNORE their
   arguments and just return the already-built instance:

       Bar::initSingleton(appA);   // builds Bar(appA)
       Bar::initSingleton(appB);   // does nothing, returns the same Bar(appA)

 - To re-construct with different arguments, you must explicitly destroy
   the instance first:

       Bar::unitSingleton();       // destroys the current instance
       Bar::initSingleton(appB);   // builds a fresh Bar(appB)

 - The default constructor is NEVER instantiated implicitly by ref() /
   unitSingleton() / a no-arg initSingleton(). Types with a reference
   member or any non-default-constructible member are therefore fine.

 - IMPLEMENT_SINGLETON uses std::unique_ptr ; ref() returns T& (unique
   ownership stays internal).

 - IMPLEMENT_SHARED_SINGLETON uses std::shared_ptr ; ref() and
   initSingleton() return std::shared_ptr<T> BY VALUE, so a caller
   keeping its own copy survives a later unitSingleton() call:

       auto sp = Bar::ref();       // copy of the internal shared_ptr
       Bar::unitSingleton();       // internal slot is reset
       sp->doStuff();              // OK - sp still holds the instance

 - getSingletonPtr() is the internal accessor (private); it ensures a
   single static instance pointer is shared across all template
   instantiations of initSingleton.

*/

#include <memory>
#include <cassert>

#define IMPLEMENT_SINGLETON(TTYPE)                                                     \
private:                                                                               \
    static std::unique_ptr<TTYPE>& getSingletonPtr() {                                 \
        static std::unique_ptr<TTYPE> mp_instance;                                     \
        return mp_instance;                                                            \
    }                                                                                  \
                                                                                       \
public:                                                                                \
    template <class... _Types>                                                         \
    static void initSingleton(_Types&&... aArgs) {                                     \
        getSingletonPtr().reset(new TTYPE(std::forward<_Types>(aArgs)...));            \
    }                                                                                  \
    static TTYPE& ref() {                                                              \
        auto* const ptr = getSingletonPtr().get();                                     \
        assert(ptr != nullptr && "You must call initSingleton() before use of ref()"); \
        return *ptr;                                                                   \
    }                                                                                  \
    static void unitSingleton() { getSingletonPtr().reset(); }

#define IMPLEMENT_SHARED_SINGLETON(TTYPE)                                                    \
private:                                                                                     \
    static std::shared_ptr<TTYPE>& getSingletonPtr() {                                       \
        static std::shared_ptr<TTYPE> mp_instance;                                           \
        return mp_instance;                                                                  \
    }                                                                                        \
                                                                                             \
public:                                                                                      \
    template <class... _Types>                                                               \
    static void initSingleton(_Types&&... aArgs) {                                           \
        getSingletonPtr().reset(new TTYPE(std::forward<_Types>(aArgs)...));                  \
    }                                                                                        \
    static std::shared_ptr<TTYPE> ref() {                                                    \
        auto& ptr = getSingletonPtr();                                                       \
        assert(ptr.get() != nullptr && "You must call initSingleton() before use of ref()"); \
        return ptr;                                                                          \
    }                                                                                        \
    static void unitSingleton() { getSingletonPtr().reset(); }
