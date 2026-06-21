/* SPDX-License-Identifier: LGPL-2.1-or-later */
#include "frontend.h"
#include <app.h>

#include <algorithm>
#include <cinttypes>
#include <filesystem>
#include <string>

#include <GLFW/glfw3.h>

#include <app.h>

#include <frontend/panes/imcodepane.h>
#include <frontend/panes/imnodalpane.h>
#include <frontend/panes/consolepane.h>
#ifdef USE_EMBEDDED_FRAME_PROFILER
#include <frontend/panes/improfilerpane.h>
#endif // USE_EMBEDDED_FRAME_PROFILER
#include <frontend/panes/imguifiledialogpane.h>

#define MESSAGING_CODE_INFOS 0
#define MESSAGING_LABEL_INFOS "Infos"
#define MESSAGING_CODE_WARNINGS 1
#define MESSAGING_LABEL_WARNINGS "Warnings"
#define MESSAGING_CODE_ERRORS 2
#define MESSAGING_CODE_DEBUG 3
#define MESSAGING_LABEL_ERRORS "Errors"
#define MESSAGING_LABEL_DEBUG "Debug"

bool Frontend::init() {
    ImGui::CustomStyle::ResetCustomId();
    m_buildThemes();
    ImLayout::initSingleton();
    ImLayout::ref().init("Panes", "Default Layout");
    Panes::ConsolePane::initSingleton();
    Panes::ImCodePane::initSingleton();
    Panes::ImNodalPane::initSingleton();
#ifdef USE_EMBEDDED_FRAME_PROFILER
    Panes::ImProfilerPane::initSingleton();
#endif  // USE_EMBEDDED_FRAME_PROFILER
    Panes::ImGuiFileDialogPane::initSingleton();
    ImLayout::ref().addPane(                                   //
        LayoutPaneInfos(Panes::ImNodalPane::ref(), "ImNodal")  //
            .setMenu("ImNodal")
            .setDisposalCentral()
            .setDefaultOpened(true)
            .setDefaultFocused(true));
    ImLayout::ref().addPane(                                 //
        LayoutPaneInfos(Panes::ImCodePane::ref(), "ImCode")  //
            .setMenu("ImCode")
            .setDisposalCentral()
            .setDefaultOpened(true)
            .setDefaultFocused(true));
#ifdef USE_EMBEDDED_FRAME_PROFILER
    ImLayout::ref().addPane(                                         //
        LayoutPaneInfos(Panes::ImProfilerPane::ref(), "ImProfiler")  //
            .setMenu("ImProfiler")
            .setDisposalSide("RIGHT", 0.3f));
#endif  // USE_EMBEDDED_FRAME_PROFILER
    ImLayout::ref().addPane(                                                   //
        LayoutPaneInfos(Panes::ImGuiFileDialogPane::ref(), "ImGuiFileDialog")  //
            .setMenu("ImGuiFileDialog")
            .setDisposalSide("LEFT", 0.3f));
    ImLayout::ref().addPane(                                   //
        LayoutPaneInfos(Panes::ConsolePane::ref(), "Console")  //
            .setMenu("Console")
            .setDisposalSide("BOTTOM", 0.5f));
    Messaging::initSingleton();
    Messaging::ref().AddCategory(MESSAGING_CODE_INFOS, "Infos(s)", MESSAGING_LABEL_INFOS, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
    Messaging::ref().AddCategory(MESSAGING_CODE_WARNINGS, "Warnings(s)", MESSAGING_LABEL_WARNINGS, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));
    Messaging::ref().AddCategory(MESSAGING_CODE_ERRORS, "Errors(s)", MESSAGING_LABEL_ERRORS, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
    Messaging::ref().AddCategory(MESSAGING_CODE_DEBUG, "Debug(s)", MESSAGING_LABEL_DEBUG, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));
    Messaging::ref().SetImLayout(&ImLayout::ref());
    ez::Log::ref().setStandardLogMessageFunctor([](const int &vType, const std::string &vMessage) {
        Messaging::ref().AddMessage(vMessage, vType, false, {}, {});
    });
    Messaging::sMessagePaneId = Panes::ConsolePane::ref()->getFlag();
    return ImLayout::ref().initPanes();
}

void Frontend::unit() {
    ImLayout::ref().unitPanes();
    Panes::ImGuiFileDialogPane::unitSingleton();
#ifdef USE_EMBEDDED_FRAME_PROFILER
    Panes::ImProfilerPane::unitSingleton();
#endif  // USE_EMBEDDED_FRAME_PROFILER
    Panes::ImNodalPane::unitSingleton();
    Panes::ImCodePane::unitSingleton();
    Panes::ConsolePane::unitSingleton();
    ImLayout::ref().unit();
    ImLayout::unitSingleton();
}

void Frontend::display() {
    ImGui::CustomStyle::ResetCustomId();
    m_rect = ImRect(ImVec2(), ImGui::GetIO().DisplaySize);
    m_actionsSystem.runImmediateActions();
    m_actionsSystem.executeFirstConditionalAction();
    m_drawMainMenuBar();
    m_drawMainStatusBar();

    if (ImLayout::ref().beginDockSpace(ImGuiDockNodeFlags_PassthruCentralNode)) {
        ImLayout::ref().endDockSpace();
    }
    ImLayout::ref().drawPanes();
    ImGuiThemeHelper::ref().Draw();
    m_drawDialogs();
    ImLayout::ref().initAfterFirstDisplay(m_rect.Max);
}

void Frontend::actionCloseApp() {
    if (App::ref().isAppClosingNeeded()) {
        return;
    }
    m_actionsSystem.clear();
    m_actionsSystem.pushBackConditonalAction([this]() {
        App::ref().closeApp();
        return true;
    });
}

void Frontend::m_actionCancel() {
    m_actionsSystem.clear();
    App::ref().appClosingNeeded(false);
}

void Frontend::m_drawMainMenuBar() {
    if (!ImGui::BeginMainMenuBar()) {
        return;
    }
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Quit", "Alt+F4")) {
            GLFWwindow *win = glfwGetCurrentContext();
            if (win) {
                glfwSetWindowShouldClose(win, GLFW_TRUE);
            }
        }
        ImGui::EndMenu();
    }

    ImLayout::ref().drawMenu(m_rect.Max);
    if (ImGui::BeginMenu("Tools")) {
        if (ImGui::BeginMenu("Styles")) {
            ImGuiThemeHelper::ref().DrawMenu();
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::MenuItem("ImGui Demo", nullptr, &m_showFlags.imguiDemo);
        ImGui::MenuItem("ImGui Metrics", nullptr, &m_showFlags.imguiMetrics);
        ImGui::MenuItem("ImNodal Demo", nullptr, &m_showFlags.imnodalDemo);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("About")) {
            m_showFlags.aboutDialog = true;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void Frontend::m_drawMainStatusBar() {
    if (ImGui::BeginMainStatusBar()) {
        Messaging::ref().DrawStatusBar();
        ImGui::EndMainStatusBar();
    }
}

void Frontend::m_drawDialogs() {
    ImLayout::ref().drawDialogsAndPopups(m_rect);
    if (m_showFlags.imguiDemo) {
        ImGui::ShowDemoWindow(&m_showFlags.imguiDemo);
    }
    if (m_showFlags.imguiMetrics) {
        ImGui::ShowMetricsWindow(&m_showFlags.imguiMetrics);
    }
    if (m_showFlags.imnodalDemo) {
        ImNodal::ShowDemoWindow(&m_showFlags.imnodalDemo);
    }
    m_drawAboutDialog();
}

void Frontend::m_drawAboutDialog() {
    if (m_showFlags.aboutDialog) {
        ImGui::OpenPopup("About");
        m_showFlags.aboutDialog = false;
    }
    if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse)) {
        constexpr ImGuiTableFlags k_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit;
        if (ImGui::BeginTable("about_tbl", 2, k_flags)) {
            ImGui::TableSetupColumn("Component", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Build", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Dear ImGui");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(IMGUI_VERSION);
            ImGui::EndTable();
        }
        ImGui::Separator();
        if (ImGui::ContrastedButton_For_Dialogs("Close", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}

ez::xml::Nodes Frontend::getXmlNodes(const std::string &aUserDatas) {
    ez::xml::Node root;
    auto &datasNode = root.addChild("datas");
    datasNode.addChild("last_open_path").setContent(getDatas().lastOpenPath);
    auto &xml_panes = root.addChild("panes");
    xml_panes.addChilds(Panes::ConsolePane::ref()->getXmlNodes(aUserDatas));
    root.addChilds(ImLayout::ref().getXmlNodes(aUserDatas));
    root.addChild("places").setContent(ImGuiFileDialog::ref().SerializePlaces());
    root.addChilds(ImGuiThemeHelper::ref().getXmlNodes(aUserDatas));
    auto &flagNode = root.addChild("flags");
    flagNode.addChild("imgui_demo").setContent(m_showFlags.imguiDemo);
    flagNode.addChild("imgui_metrics").setContent(m_showFlags.imguiMetrics);
    flagNode.addChild("imnodal_demo").setContent(m_showFlags.imnodalDemo);
    return root.getChildren();
}

bool Frontend::setFromXmlNodes(const ez::xml::Node &aNode, const ez::xml::Node &aParent, const std::string &aUserDatas) {
    const auto &name{aNode.getName()};
    const auto &parentName{aParent.getName()};
    ImLayout::ref().setFromXmlNodes(aNode, aParent, aUserDatas);
    ImGuiThemeHelper::ref().setFromXmlNodes(aNode, aParent, aUserDatas);
    Panes::ConsolePane::ref()->setFromXmlNodes(aNode, aParent, aUserDatas);
    if (parentName == "flags") {
        if (name == "imgui_demo") {
            m_showFlags.imguiDemo = aNode.getContent<bool>();
        } else if (name == "imgui_metrics") {
            m_showFlags.imguiMetrics = aNode.getContent<bool>();
        } else if (name == "imnodal_demo") {
            m_showFlags.imnodalDemo = aNode.getContent<bool>();
        }
    } else {
        if (name == "places") {
            ImGuiFileDialog::ref().DeserializePlaces(aNode.getContent());
        } else if (name == "last_open_path") {
            getDatasRef().lastOpenPath = aNode.getContent();
        }
    }
    return true;
}
