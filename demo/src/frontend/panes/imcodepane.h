#pragma once

#include <frontend/panes/abstract/apane.h>

#include <imguipack.h>

namespace Panes {

class ImCodePane : public APane {
    DISABLE_CONSTRUCTORS(ImCodePane)
    DISABLE_DESTRUCTORS(ImCodePane)
    IMPLEMENT_SHARED_SINGLETON(ImCodePane)
private:
    im::Code m_editor;

public:
    bool init() override;
    void unit() override;
    bool drawPanes(bool* apOpened, LayoutPaneUserDatas apUserDatas) override;
    ez::xml::Nodes getXmlNodes(const std::string& aUserDatas) override;
    bool setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) override;
};

}  // namespace pane
