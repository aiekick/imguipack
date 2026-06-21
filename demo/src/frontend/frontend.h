/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once
#include <string>
#include <vector>
#include <imguipack.h>
#include <ezlibs/ezClass.hpp>
#include <ezlibs/ezSingleton.hpp>
#include <ezlibs/ezXmlConfig.hpp>
#include <ezlibs/ezActions.hpp>

class Frontend : public ez::xml::Config {
    DISABLE_CONSTRUCTORS(Frontend)
    DISABLE_DESTRUCTORS(Frontend)
    IMPLEMENT_SINGLETON(Frontend)

private:
    ImRect m_rect{};
    struct ImGuiDemoFlags {
        bool imguiDemo{false};
        bool imguiMetrics{false};
        bool imnodalDemo{false};
        bool aboutDialog{false};
    } m_showFlags;
    ez::Actions m_actionsSystem;

    struct Datas {
        std::string lastOpenPath{"."};
    } m_datas;
    DATAS_STRUCT_GETTER(Datas, Datas, m_datas);

public:
    bool init();
    void unit();
    void display();
    void actionCloseApp();

    ez::xml::Nodes getXmlNodes(const std::string& aUserDatas) override;
    bool setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) override;

private:
    bool m_buildThemes();
    void m_drawMainMenuBar();
    void m_drawMainStatusBar();
    void m_drawDialogs();
    void m_drawAboutDialog();
    void m_actionCancel();
};
