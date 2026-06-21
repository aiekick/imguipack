#include <TestImCode.h>
#include <ezlibs/ezCTest.hpp>

#include <imguipack.h>

#include <exception>
#include <iostream>
#include <string>
#include <array>
#include <vector>

using namespace im;

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

static void setContent(Code& aEditor, const std::string& aText) {
    aEditor.setText(aText.data(), (uint64_t)aText.size());
}

static int32_t tokenColorAt(const std::vector<Code::Token>& aTokens, int32_t aColumn) {
    for (size_t i = 0; i < aTokens.size(); ++i) {
        if (aTokens[i].startColumn == aColumn) return aTokens[i].color;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Text model: setText / getText round-trips
// ---------------------------------------------------------------------------
bool TestImCode_TextRoundTrip() {
    Code editor;
    editor.init();

    setContent(editor, "line1\nline2\nline3");
    CTEST_ASSERT(editor.getText() == "line1\nline2\nline3");

    setContent(editor, "a\r\nb\r\n");  // CRLF normalized to LF, trailing empty line
    CTEST_ASSERT(editor.getText() == "a\nb\n");

    setContent(editor, "");
    CTEST_ASSERT(editor.getText() == "");

    setContent(editor, "single");
    CTEST_ASSERT(editor.getText() == "single");
    return true;
}

// ---------------------------------------------------------------------------
// Cursor clamping + insertText at caret
// ---------------------------------------------------------------------------
bool TestImCode_Insert() {
    Code editor;
    editor.init();

    setContent(editor, "hello");
    editor.setCursor(Code::Pos{0, 5});
    editor.insertText(" world");
    CTEST_ASSERT(editor.getText() == "hello world");
    CTEST_ASSERT(editor.getCursor().column == 11);

    // newline insertion splits the line and moves the caret to the new line
    setContent(editor, "ab");
    editor.setCursor(Code::Pos{0, 1});
    editor.insertText("\n");
    CTEST_ASSERT(editor.getText() == "a\nb");
    CTEST_ASSERT(editor.getCursor().line == 1 && editor.getCursor().column == 0);

    // cursor beyond end-of-line is clamped
    setContent(editor, "ab");
    editor.setCursor(Code::Pos{0, 999});
    CTEST_ASSERT(editor.getCursor().column == 2);
    return true;
}

// ---------------------------------------------------------------------------
// Backspace / Delete (line merges) via the public command API
// ---------------------------------------------------------------------------
bool TestImCode_BackspaceDelete() {
    Code editor;
    editor.init();

    setContent(editor, "abc");
    editor.setCursor(Code::Pos{0, 2});
    editor.execute(Code::Command::Backspace);
    CTEST_ASSERT(editor.getText() == "ac");

    // backspace at column 0 merges with the previous line
    setContent(editor, "a\nb");
    editor.setCursor(Code::Pos{1, 0});
    editor.execute(Code::Command::Backspace);
    CTEST_ASSERT(editor.getText() == "ab");
    CTEST_ASSERT(editor.getCursor().line == 0 && editor.getCursor().column == 1);

    // delete forward at end-of-line merges the next line
    setContent(editor, "a\nb");
    editor.setCursor(Code::Pos{0, 1});
    editor.execute(Code::Command::DeleteForward);
    CTEST_ASSERT(editor.getText() == "ab");
    return true;
}

// ---------------------------------------------------------------------------
// Selection + replace
// ---------------------------------------------------------------------------
bool TestImCode_Selection() {
    Code editor;
    editor.init();

    setContent(editor, "hello world");
    editor.setSelection(Code::Range{Code::Pos{0, 0}, Code::Pos{0, 5}});
    CTEST_ASSERT(editor.getSelectedText() == "hello");

    editor.insertText("hi");  // typing replaces the selection
    CTEST_ASSERT(editor.getText() == "hi world");

    // multi-line selection text
    setContent(editor, "ab\ncd");
    editor.setSelection(Code::Range{Code::Pos{0, 1}, Code::Pos{1, 1}});
    CTEST_ASSERT(editor.getSelectedText() == "b\nc");
    return true;
}

// ---------------------------------------------------------------------------
// Undo / redo (with coalesced typing)
// ---------------------------------------------------------------------------
bool TestImCode_UndoRedo() {
    Code editor;
    editor.init();

    setContent(editor, "hello");
    editor.setCursor(Code::Pos{0, 5});
    editor.insertText(" world");
    CTEST_ASSERT(editor.getText() == "hello world");

    editor.execute(Code::Command::Undo);
    CTEST_ASSERT(editor.getText() == "hello");

    editor.execute(Code::Command::Redo);
    CTEST_ASSERT(editor.getText() == "hello world");

    // round-trip invariant: edit then undo restores the original exactly
    setContent(editor, "0123456789");
    const std::string original = editor.getText();
    editor.setCursor(Code::Pos{0, 5});
    editor.insertText("XYZ");
    editor.execute(Code::Command::Backspace);
    editor.execute(Code::Command::Undo);  // undo backspace
    editor.execute(Code::Command::Undo);  // undo insert
    CTEST_ASSERT(editor.getText() == original);
    return true;
}

// ---------------------------------------------------------------------------
// Indent / dedent block
// ---------------------------------------------------------------------------
bool TestImCode_Indent() {
    Code editor;
    editor.init();

    setContent(editor, "a\nb");
    editor.setSelection(Code::Range{Code::Pos{0, 0}, Code::Pos{1, 1}});
    editor.execute(Code::Command::IndentLines);
    CTEST_ASSERT(editor.getText() == "    a\n    b");  // default tabWidth == 4

    editor.execute(Code::Command::OutdentLines);
    CTEST_ASSERT(editor.getText() == "a\nb");
    return true;
}

// ---------------------------------------------------------------------------
// Lexer: C / C++ (classification + cross-line block comment)
// ---------------------------------------------------------------------------
bool TestImCode_LexerCpp() {
    Code editor;
    editor.init();
    editor.setLanguage("cpp");
    std::vector<Code::Token> tokens;

    setContent(editor, "int x = 42; // hi");
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(tokenColorAt(tokens, 0) == Code::Col_Type);        // int
    CTEST_ASSERT(tokenColorAt(tokens, 4) == Code::Col_Identifier);  // x
    CTEST_ASSERT(tokenColorAt(tokens, 8) == Code::Col_Number);      // 42
    CTEST_ASSERT(tokenColorAt(tokens, 12) == Code::Col_Comment);    // // hi

    // block comment spanning two lines: the lexer state must carry over
    setContent(editor, "/* a\nb */ int");
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(!tokens.empty() && tokens[0].color == Code::Col_Comment);
    editor.getLineTokens(1, tokens);
    bool hasComment = false;
    bool hasType = false;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].color == Code::Col_Comment) hasComment = true;
        if (tokens[i].color == Code::Col_Type) hasType = true;
    }
    CTEST_ASSERT(hasComment && hasType);
    return true;
}

// ---------------------------------------------------------------------------
// Lexer: GLSL (same C-like structure, different vocabulary)
// ---------------------------------------------------------------------------
bool TestImCode_LexerGlsl() {
    Code editor;
    editor.init();
    editor.setLanguage("glsl");
    std::vector<Code::Token> tokens;

    setContent(editor, "uniform vec3 color;");
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(tokenColorAt(tokens, 0) == Code::Col_Keyword);      // uniform
    CTEST_ASSERT(tokenColorAt(tokens, 8) == Code::Col_Type);         // vec3
    CTEST_ASSERT(tokenColorAt(tokens, 13) == Code::Col_Identifier);  // color
    return true;
}

// ---------------------------------------------------------------------------
// Lexer: Lua (-- line comment, keyword, multi-line long comment)
// ---------------------------------------------------------------------------
bool TestImCode_LexerLua() {
    Code editor;
    editor.init();
    editor.setLanguage("lua");
    std::vector<Code::Token> tokens;

    setContent(editor, "local x = 42 -- note");
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(tokenColorAt(tokens, 0) == Code::Col_Keyword);     // local
    CTEST_ASSERT(tokenColorAt(tokens, 6) == Code::Col_Identifier);  // x
    CTEST_ASSERT(tokenColorAt(tokens, 13) == Code::Col_Comment);    // -- note

    // long comment --[[ ... ]] spanning two lines
    setContent(editor, "--[[ c\nd ]] local");
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(!tokens.empty() && tokens[0].color == Code::Col_Comment);
    editor.getLineTokens(1, tokens);
    bool hasComment = false;
    bool hasKeyword = false;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].color == Code::Col_Comment) hasComment = true;
        if (tokens[i].color == Code::Col_Keyword) hasKeyword = true;
    }
    CTEST_ASSERT(hasComment && hasKeyword);
    return true;
}

// ---------------------------------------------------------------------------
// Lexer: SQL (case-insensitive keywords)
// ---------------------------------------------------------------------------
bool TestImCode_LexerSql() {
    Code editor;
    editor.init();
    editor.setLanguage("sql");
    std::vector<Code::Token> tokens;

    setContent(editor, "select id from users; -- q");
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(tokenColorAt(tokens, 0) == Code::Col_Keyword);   // select (lowercase)
    CTEST_ASSERT(tokenColorAt(tokens, 10) == Code::Col_Keyword);  // from
    CTEST_ASSERT(tokenColorAt(tokens, 22) == Code::Col_Comment);  // -- q

    setContent(editor, "SELECT");  // uppercase colorizes the same
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(tokenColorAt(tokens, 0) == Code::Col_Keyword);
    return true;
}

// ---------------------------------------------------------------------------
// Lexer: unsupported language -> plain text (no tokens)
// ---------------------------------------------------------------------------
bool TestImCode_LexerPlain() {
    Code editor;
    editor.init();
    std::vector<Code::Token> tokens;

    setContent(editor, "int x");
    editor.setLanguage("python");
    editor.getLineTokens(0, tokens);
    CTEST_ASSERT(tokens.empty());
    return true;
}

// ---------------------------------------------------------------------------
// Find: next match navigation (wraps)
// ---------------------------------------------------------------------------
bool TestImCode_FindNext() {
    Code editor;
    editor.init();
    setContent(editor, "abc abc abc");
    editor.setSearch("abc", Code::FindFlags_None);
    CTEST_ASSERT(editor.searchMatchCount() == 3);

    editor.setCursor(Code::Pos{0, 0});
    CTEST_ASSERT(editor.findNext());
    CTEST_ASSERT(editor.getSelectedText() == "abc");
    CTEST_ASSERT(editor.getSelection().start.column == 0);
    CTEST_ASSERT(editor.findNext());
    CTEST_ASSERT(editor.getSelection().start.column == 4);
    CTEST_ASSERT(editor.findNext());
    CTEST_ASSERT(editor.getSelection().start.column == 8);
    CTEST_ASSERT(editor.findNext());  // wraps to the first match
    CTEST_ASSERT(editor.getSelection().start.column == 0);
    return true;
}

// ---------------------------------------------------------------------------
// Find: case-insensitive flag
// ---------------------------------------------------------------------------
bool TestImCode_FindCaseInsensitive() {
    Code editor;
    editor.init();
    setContent(editor, "Foo foo FOO");
    editor.setSearch("foo", Code::FindFlags_None);
    CTEST_ASSERT(editor.searchMatchCount() == 1);  // exact case only
    editor.setSearch("foo", Code::FindFlags_CaseInsensitive);
    CTEST_ASSERT(editor.searchMatchCount() == 3);
    return true;
}

// ---------------------------------------------------------------------------
// Replace all
// ---------------------------------------------------------------------------
bool TestImCode_ReplaceAll() {
    Code editor;
    editor.init();
    setContent(editor, "a x a x a");
    editor.setSearch("x", Code::FindFlags_None);
    const int32_t count = editor.replaceAll("YY");
    CTEST_ASSERT(count == 2);
    CTEST_ASSERT(editor.getText() == "a YY a YY a");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestImCode(const std::string& vTest) {
    IfTestExist(TestImCode_TextRoundTrip);
    else IfTestExist(TestImCode_Insert);
    else IfTestExist(TestImCode_BackspaceDelete);
    else IfTestExist(TestImCode_Selection);
    else IfTestExist(TestImCode_UndoRedo);
    else IfTestExist(TestImCode_Indent);
    else IfTestExist(TestImCode_LexerCpp);
    else IfTestExist(TestImCode_LexerGlsl);
    else IfTestExist(TestImCode_LexerLua);
    else IfTestExist(TestImCode_LexerSql);
    else IfTestExist(TestImCode_LexerPlain);
    else IfTestExist(TestImCode_FindNext);
    else IfTestExist(TestImCode_FindCaseInsensitive);
    else IfTestExist(TestImCode_ReplaceAll);
    return false;
}

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
