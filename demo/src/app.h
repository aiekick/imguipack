/*
 * This file is part of cdpViewer.
 *
 * Copyright (C) 2025 Stephane Cuillerdier (aka aiekick)
 *
 * cdpViewer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cdpViewer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with cdpViewer.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <ezlibs/ezApp.hpp>
#include <ezlibs/ezClass.hpp>
#include <ezlibs/ezXmlConfig.hpp>
#include <ezlibs/ezSingleton.hpp>

struct GLFWwindow;
class App : public ez::xml::Config {
    DISABLE_CONSTRUCTORS(App)
    DISABLE_DESTRUCTORS(App)
    IMPLEMENT_SINGLETON(App)
public:
#ifdef __EMSCRIPTEN__
    static void render(void* vApp);
#endif

private:
    std::unique_ptr<ez::App> mp_app;
    GLFWwindow* mp_mainWindow{nullptr};
    const char* m_glslVersion{""};
    bool m_consoleVisiblity{false};
    bool m_appClosingNeeded{false};

public:
    bool init(int argc, char** argv);
    void run();
    void unit();
    void closeApp();
    bool isAppClosingNeeded();
    void appClosingNeeded(const bool& aFlag);
    void setAppTitle(const std::string& aTitle);
    ez::xml::Nodes getXmlNodes(const std::string& aUserDatas) override;
    bool setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) override;

private:
    void m_render();
    bool m_initGlfw();
    void m_unitGlfw();
    bool m_initImgui();
    void m_unitImgui();
};
