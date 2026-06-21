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
#include <frontend/panes/improfilerpane.h>
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
    Panes::ImProfilerPane::initSingleton();
    Panes::ImGuiFileDialogPane::initSingleton();
    ImLayout::ref().addPane(LayoutPaneInfos(Panes::ConsolePane::ref(), "Console").setMenu("Console").setDisposalSide("BOTTOM", 0.5f));
    ImLayout::ref().addPane(LayoutPaneInfos(Panes::ImCodePane::ref(), "ImCode").setMenu("ImCode").setDisposalSide("RIGHT", 0.3f).setDefaultOpened(true));
    ImLayout::ref().addPane(LayoutPaneInfos(Panes::ImNodalPane::ref(), "ImNodal").setMenu("ImNodal").setDisposalSide("BOTTOM", 0.5f));
    ImLayout::ref().addPane(LayoutPaneInfos(Panes::ImProfilerPane::ref(), "ImProfiler").setMenu("ImProfiler").setDisposalSide("RIGHT", 0.3f));
    ImLayout::ref().addPane(LayoutPaneInfos(Panes::ImGuiFileDialogPane::ref(), "ImGuiFileDialog").setMenu("ImGuiFileDialog").setDisposalSide("LEFT", 0.3f).setDefaultOpened(true));
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
    Panes::ImProfilerPane::unitSingleton();
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
        if (ImGui::MenuItem("Open cdp")) {
            IGFD::FileDialogConfig config;
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_Modal;
            config.path = getDatas().lastOpenPath;
            ImGuiFileDialog::ref().OpenDialog("OPEN_CDP_FILE", "Open a cdp file", ".cdp", config);
        }
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
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo", nullptr, &m_showFlags.imgui);
            ImGui::MenuItem("ImNodal Demo", nullptr, &m_showFlags.imnodal);
            ImGui::EndMenu();
        }
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
    if (m_showFlags.imgui) {
        ImGui::ShowDemoWindow(&m_showFlags.imgui);
    }
    if (m_showFlags.imnodal) {
        ImNodal::ShowDemoWindow(&m_showFlags.imnodal);
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
    return root.getChildren();
}

bool Frontend::setFromXmlNodes(const ez::xml::Node &aNode, const ez::xml::Node &aParent, const std::string &aUserDatas) {
    ImLayout::ref().setFromXmlNodes(aNode, aParent, aUserDatas);
    ImGuiThemeHelper::ref().setFromXmlNodes(aNode, aParent, aUserDatas);
    Panes::ConsolePane::ref()->setFromXmlNodes(aNode, aParent, aUserDatas);
    if (aNode.getName() == "places") {
        ImGuiFileDialog::ref().DeserializePlaces(aNode.getContent());
    } else if (aNode.getName() == "last_open_path") {
        getDatasRef().lastOpenPath = aNode.getContent();
    }
    return true;
}
