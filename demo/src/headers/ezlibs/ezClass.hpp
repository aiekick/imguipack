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

// ezClass is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

/*
this macro disable constructors of class
*/
#define DISABLE_CONSTRUCTORS(TTYPE)          \
public:                                      \
    TTYPE() = default;                       \
    TTYPE(const TTYPE&) = delete;            \
    TTYPE& operator=(const TTYPE&) = delete; \
    TTYPE(TTYPE&&) = delete;                 \
    TTYPE& operator=(TTYPE&&) = delete;

/*
this macro disable destructors of class
*/
#define DISABLE_DESTRUCTORS(TTYPE) \
public:                            \
    virtual ~TTYPE() = default;

/*
DATAS_GETTER(LocalDatas, Datas, m_datas)
will give :
    LocalDatas& getDatasRef() {
        return m_datas;
    }
    const LocalDatas& getDatas() const{
        return m_datas;
    }
*/
#define DATAS_STRUCT_GETTER(TTYPE, TNAME, TDATAS) \
public:                                           \
    TTYPE& get##TNAME##Ref() {                    \
        return TDATAS;                            \
    }                                             \
    const TTYPE& get##TNAME() const {             \
        return TDATAS;                            \
    }

/*
DATAS_ISER(Datas, m_datas)
will give :
    bool isDatas() const {
        return m_datas;
    }
*/
#define DATAS_ISER(TNAME, TDATAS) \
public:                           \
    bool is##TNAME() const {      \
        return TDATAS;            \
    }

/*
DATAS_ISER_REF(Datas, m_datas)
will give :
    bool& isDatasRef() {
        return m_datas;
    }
*/
#define DATAS_ISER_REF(TNAME, TDATAS) \
public:                               \
    bool& is##TNAME##Ref() {          \
        return TDATAS;                \
    }

/*
DATAS_SETTER(LocalDatas, Datas, m_datas)
will give :
    void setDatas(const LocalDatas& vDatas) {
        TDATAS = vDatas;
    }
*/
#define DATAS_SETTER(TTYPE, TNAME, TDATAS)   \
public:                                      \
    void set##TNAME(const TTYPE& v##TNAME) { \
        TDATAS = v##TNAME;                   \
    }
