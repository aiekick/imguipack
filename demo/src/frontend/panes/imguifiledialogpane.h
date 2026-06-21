#pragma once

#include <frontend/panes/abstract/apane.h>

namespace Panes {

class ImGuiFileDialogPane : public APane {
    DISABLE_CONSTRUCTORS(ImGuiFileDialogPane)
    DISABLE_DESTRUCTORS(ImGuiFileDialogPane)
    IMPLEMENT_SHARED_SINGLETON(ImGuiFileDialogPane)
public:
    bool init() override;
    void unit() override;
    bool drawPanes(bool* apOpened, LayoutPaneUserDatas apUserDatas) override;
    ez::xml::Nodes getXmlNodes(const std::string& aUserDatas) override;
    bool setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) override;
};

}  // namespace pane
