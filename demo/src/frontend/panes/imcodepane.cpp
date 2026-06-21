#include "imcodepane.h"

// monospace ImFont for the editor demo, created in App::m_initImgui (app.cpp)
ImFont* GImCodeDemoMonoFont = nullptr;

namespace Panes {

bool ImCodePane::init() {
    const std::string code = R"(// ImCode demo
#include <cstdio>

int main() {
    for (int i = 0; i < 10; ++i) {
        printf(\"hello %d\\n\", i);
    }
    return 0;
};
)";
    m_editor.init();
    m_editor.getStyle().font = GImCodeDemoMonoFont;
    m_editor.setLanguage("cpp");
    m_editor.setMarkers({{3, IM_COL32(220, 80, 80, 255), "breakpoint", 0}});
    m_editor.setDecorations({{{{4, 8}, {4, 11}}, im::Code::DecoKind::Squiggle, IM_COL32(90, 170, 255, 255), "the loop index"}});
    m_editor.setDiagnostics({{{{1, 0}, {1, 17}}, im::Code::Severity::Warning, "unused include"}});
    m_editor.setText(code.data(), (uint64_t)code.size());
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
            m_editor.Render("##imcode_editor", ImGui::GetContentRegionAvail());
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
