/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* This header is force-included by Dear ImGui (and by imguipack.h) at the
 * start of every translation unit through the IMGUI_USER_CONFIG macro
 * defined globally in CMakeLists.txt. Its job is to pull in ezlibs headers
 * that imguipack/ImLayout/ImGuiThemeHelper rely on when their compile-time
 * options are active — without this we hit "ez::xml::Config not declared"
 * errors inside the imguipack build itself. */

#include <memory>          /* unique_ptr in ImGuiThemeHelper / ImLayout */

#include <ezlibs/ezTools.hpp>
#include <ezlibs/ezXmlConfig.hpp>
