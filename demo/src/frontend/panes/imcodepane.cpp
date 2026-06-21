#include "imcodepane.h"

// monospace ImFont for the editor demo, created in App::m_initImgui (app.cpp)
ImFont* GImCodeDemoMonoFont = nullptr;

namespace Panes {

bool ImCodePane::init() {
    return true;
}

void ImCodePane::unit() {
}

bool ImCodePane::drawPanes(bool* apOpened, LayoutPaneUserDatas apUserDatas) {
    bool change = false;
    if (apOpened != nullptr && *apOpened) {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
        if (ImGui::Begin(getName().c_str(), apOpened, flags)) {
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
            auto win = ImGui::GetCurrentWindowRead();
            if (win->Viewport->Idx != 0)
                flags |= ImGuiWindowFlags_NoResize;  // | ImGuiWindowFlags_NoTitleBar;
            else
                flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;
#endif
            // demo-only: a single editor instance, lazily initialized (step 1 wiring)
            static const std::string kDemoCode =
                "// ImCode demo\n"
                "#include <cstdio>\n"
                "\n"
                "int main() {\n"
                "    for (int i = 0; i < 10; ++i) {\n"
                "        printf(\"hello %d\\n\", i);\n"
                "    }\n"
                "    return 0;\n"
                "}\n";
            static im::Code s_editor;
            static bool s_editorReady = false;
            if (!s_editorReady) {
                s_editor.init();
                s_editor.getStyle().font = GImCodeDemoMonoFont;
                s_editor.setLanguage("cpp");
                s_editor.setText(kDemoCode.data(), (uint64_t)kDemoCode.size());
                s_editorReady = true;
            }
            s_editor.Render("##imcode_editor", ImGui::GetContentRegionAvail());
        }
        ImGui::End();
    }
    return change;
}

ez::xml::Nodes ImCodePane::getXmlNodes(const std::string& aUserDatas) {
    ez::xml::Node node;
    return node.getChildren();
}

bool ImCodePane::setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) {
    return false;  // leaf dont explore childs
}

}  // namespace pane
