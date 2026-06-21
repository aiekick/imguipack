#include "improfilerpane.h"

namespace Panes {

bool ImProfilerPane::init() {
    return true;
}

void ImProfilerPane::unit() {
}

bool ImProfilerPane::drawPanes(bool* apOpened, LayoutPaneUserDatas apUserDatas) {
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
        }
        ImGui::End();
    }
    return change;
}

ez::xml::Nodes ImProfilerPane::getXmlNodes(const std::string& aUserDatas) {
    ez::xml::Node node;
    return node.getChildren();
}

bool ImProfilerPane::setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) {
    return false;  // leaf dont explore childs
}

}  // namespace pane
