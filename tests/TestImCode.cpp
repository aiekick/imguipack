#include <TestImCode.h>
#include <ezlibs/ezCTest.hpp>

#include <imguipack.h>

#include <exception>
#include <iostream>
#include <string>
#include <array>

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
