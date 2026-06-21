/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <imguipack.h>
#include <ezlibs/ezClass.hpp>
#include <ezlibs/ezSingleton.hpp>
#include <ezlibs/ezXmlConfig.hpp>

class APane : public AbstractPane, public ez::xml::Config {
public:
    bool init() override {
        return true;
    }
    void unit() override {
    }

    bool drawPanes(bool * /* apOpened */, LayoutPaneUserDatas /*apUserDatas*/) override {
        return false;
    }
    bool drawWidgets(LayoutPaneUserDatas /*apUserDatas*/) override {
        return false;
    }
    bool drawOverlays(const ImRect & /*aRect*/, LayoutPaneUserDatas /*apUserDatas*/) override {
        return false;
    }
    bool drawDialogsAndPopups(const ImRect & /*aRect*/, LayoutPaneUserDatas /*apUserDatas*/) override {
        return false;
    }
    bool canBeDisplayed() override {
        return true;
    }

    ez::xml::Nodes getXmlNodes(const std::string &aUserDatas) override = 0;
    bool setFromXmlNodes(const ez::xml::Node &aNode, const ez::xml::Node &aParent, const std::string &aUserDatas) override = 0;
};
