#include "consolepane.h"

namespace Panes {

bool ConsolePane::init() {
    return true;
}

void ConsolePane::unit() {
}

bool ConsolePane::drawPanes(bool* apOpened, LayoutPaneUserDatas apUserDatas) {
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
            Messaging::ref().DrawConsolePane();
        }
        ImGui::End();
    }
    return change;
}

ez::xml::Nodes ConsolePane::getXmlNodes(const std::string& aUserDatas) {
    ez::xml::Node node;
    return node.getChildren();
}

bool ConsolePane::setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) {
    return false;  // leaf dont explore childs
}

}  // namespace pane
