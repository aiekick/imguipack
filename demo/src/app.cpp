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

#include "app.h"

#include <ezlibs/ezLog.hpp>
#include <ezlibs/ezOS.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif 

#include <GLFW/glfw3.h>

#if defined(WINDOWS_OS) && !defined(__EMSCRIPTEN__)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include <headers/version.h>

#include <backend/backend.h>
#include <frontend/frontend.h>

#include <imguipack.h>

#define IMGUI_IMPL_API
#include <3rdparty/imgui_docking/backends/imgui_impl_opengl3.h>
#include <3rdparty/imgui_docking/backends/imgui_impl_glfw.h>

// we include the cpp just for embedded fonts
#include <resources/fonts/roboto.h>
#include <resources/fonts/code.h>

static void glfw_error_callback(int aError, const char* aDescription) {
    const std::string desc{aDescription};
    if (desc.find("Failed to convert clipboard to string") != std::string::npos) {
        return;
    }
    LogVarError("glfw error %i : %s", aError, aDescription);
}

static void glfw_window_close_callback(GLFWwindow* ap_win) {
    glfwSetWindowShouldClose(ap_win, GLFW_FALSE);  // block app closing
    Frontend::ref().actionCloseApp();
}

static void glfw_window_drop_callback(GLFWwindow* ap_win, int a_count, const char** app_paths) {
    if (a_count <= 0 || app_paths == nullptr || app_paths[0] == nullptr) {
        return;
    }
    std::vector<std::string> paths;
    paths.resize(a_count);
    for (int idx = 0; idx < a_count; ++idx) {
        paths[idx] = app_paths[idx];
    }
    Backend::ref().justDropFiles(paths);
}

bool App::init(int argc, char** argv) {
    mp_app = std::make_unique<ez::App>(argc, argv);
    printf("-----------\n");
    printf("[[ %s Beta %s ]]\n", cdpViewer_Label, cdpViewer_BuildId);
    auto loc = std::setlocale(LC_ALL, ".UTF8");
    if (!loc) {
        printf("setlocale fail to apply with this compiler. it seems the unicode will be NOK\n");
    }
    bool ret = true;
    ret &= m_initGlfw();
    ret &= m_initImgui();
    Backend::initSingleton();
    Frontend::initSingleton();
    ret &= Backend::ref().init();
    ret &= Frontend::ref().init();
    LoadConfigFile("config.xml", "app");
    return ret;
}

void App::unit() {
    SaveConfigFile("config.xml", "app", "config");
    Frontend::ref().unit();
    Backend::ref().unit();
    Frontend::unitSingleton();
    Backend::unitSingleton();
    m_unitImgui();
    m_unitGlfw();
}

bool App::isAppClosingNeeded() {
    return m_appClosingNeeded;
}

void App::appClosingNeeded(const bool& aFlag) {
    m_appClosingNeeded = aFlag;
}

void App::closeApp() {
#ifndef __EMSCRIPTEN__
    glfwSetWindowShouldClose(mp_mainWindow, 1);
#endif
}

void App::setAppTitle(const std::string& aTitle) {
    std::string title = ez::str::toStr("%s Beta %s", cdpViewer_Label, cdpViewer_BuildId);
    if (!aTitle.empty()) {
        title = ez::str::toStr("%s - %s", title.c_str(), aTitle.c_str());
    }
    glfwSetWindowTitle(mp_mainWindow, title.c_str());
}

#ifdef __EMSCRIPTEN__
static void App::render(void* aApp) {
    auto* app = static_cast<App*>(aApp);
    app->render();
}
#endif

void App::run() {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(App::render, this, 0, true);
#else
    while (!glfwWindowShouldClose(mp_mainWindow)) {
        m_render();
    }
#endif
}

ez::xml::Nodes App::getXmlNodes(const std::string& aUserDatas) {
    ez::xml::Node root;
    root.addChilds(Backend::ref().getXmlNodes(aUserDatas));
    root.addChilds(Frontend::ref().getXmlNodes(aUserDatas));
    return root.getChildren();
}

bool App::setFromXmlNodes(const ez::xml::Node& aNode, const ez::xml::Node& aParent, const std::string& aUserDatas) {
    Backend::ref().setFromXmlNodes(aNode, aParent, aUserDatas);
    Frontend::ref().setFromXmlNodes(aNode, aParent, aUserDatas);
    return true;
}

void App::m_render() {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
#ifndef __EMSCRIPTEN__
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Viewport
#endif

#ifndef __EMSCRIPTEN__
    glfwPollEvents();
#endif

    Backend::ref().startFrame();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImNodal::NewFrame();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    Frontend::ref().display();

    ImGui::Render();
    glViewport(0, 0, viewport->WorkSize.x, viewport->WorkSize.y);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifndef __EMSCRIPTEN__
    auto* backup_current_context = glfwGetCurrentContext();
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    glfwMakeContextCurrent(backup_current_context);
#endif

    glfwSwapBuffers(mp_mainWindow);

    Backend::ref().endFrame();
}

bool App::m_initGlfw() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return false;
    }

#ifdef __EMSCRIPTEN__
    // GL 3.0 + GLSL 130
    m_glslVersion = "#version 300 es";
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
#else
    // GL 3.0 + GLSL 130
    m_glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif

    // Create window with graphics context
    mp_mainWindow = glfwCreateWindow(1280, 720, cdpViewer_Label, nullptr, nullptr);
    if (mp_mainWindow == nullptr) {
        std::cout << "Fail to create the window" << std::endl;
        return false;
    }
    glfwMakeContextCurrent(mp_mainWindow);
    glfwSwapInterval(1);  // Enable vsync

#ifndef __EMSCRIPTEN__
    if (gladLoadGL() == 0) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return false;
    }
#endif

    glfwSetWindowCloseCallback(mp_mainWindow, glfw_window_close_callback);
    glfwSetDropCallback(mp_mainWindow, glfw_window_drop_callback);

    return true;
}

void App::m_unitGlfw() {
    glfwDestroyWindow(mp_mainWindow);
    glfwTerminate();
}

bool App::m_initImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    IMNODAL_CHECKVERSION();
    ImNodal::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
#ifndef __EMSCRIPTEN__
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Viewport
#endif
    // io.FontAllowUserScaling = true;                      // activate zoom feature with ctrl + mousewheel
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
    io.ConfigViewportsNoDecoration = false;  // toujours mettre une frame aux fenetres enfants
#endif

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    ImFont* robotoFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_BUFFER_NAME_RM, 15.0f);
    assert(robotoFont != nullptr);
    ImFont* codeFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_BUFFER_NAME_CODE, 15.0f);
    assert(codeFont != nullptr);
    // ImCode editor uses the monospace code font; Roboto stays the UI default
    extern ImFont* GImCodeDemoMonoFont;  // defined in imcodepane.cpp
    GImCodeDemoMonoFont = codeFont;
    ImGui::GetIO().FontDefault = robotoFont;

    return ImGui_ImplGlfw_InitForOpenGL(mp_mainWindow, true) && ImGui_ImplOpenGL3_Init(m_glslVersion);
}

void App::m_unitImgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImNodal::DestroyContext();
    ImGui::DestroyContext();
}