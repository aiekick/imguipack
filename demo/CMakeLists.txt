cmake_minimum_required(VERSION 3.20)

set(PROJECT Demo)

project(${PROJECT} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

set(FINAL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}/debug")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}/release")
set(CMAKE_PDB_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}/minsizerel")
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}/reldeb")

set(CMAKE_INSTALL_PREFIX .)
set(CMAKE_INSTALL_BINDIR .)

if(NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX _debug)
endif()
if(NOT CMAKE_RELEASE_POSTFIX)
    set(CMAKE_RELEASE_POSTFIX) ## to config in file
endif()
if(NOT CMAKE_MINSIZEREL_POSTFIX)
    set(CMAKE_MINSIZEREL_POSTFIX _minsizerel)
endif()
if(NOT CMAKE_RELWITHDEBINFO_POSTFIX)
    set(CMAKE_RELWITHDEBINFO_POSTFIX _reldeb)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zo")
    add_definitions(
		-D_USE_MATH_DEFINES
		-DMSVC
		-DNOMINMAX
		-D_CRT_SECURE_NO_WARNINGS
	)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Wall -Wextra -Wpedantic -Wunused-parameter -Wno-unknown-pragmas")
endif ()

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_definitions(
    -DNEW_SINGLETON
    -DOPENGL_LOADER=<glad/glad.h>
    -DUSE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
    -DIMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/src/headers/customImGuiConfig.h"
    -DCUSTOM_IMGUIFILEDIALOG_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/src/headers/customImGuiFileDialogConfig.h"
)

set_target_properties(imguipack PROPERTIES FOLDER 3rdparty)
set_target_properties(freetype PROPERTIES FOLDER 3rdparty)
if(TARGET boost_regex)
	set_target_properties(boost_regex PROPERTIES FOLDER 3rdparty)
endif()
	
include(FetchContent)

include(cmake/glad.cmake)
include(cmake/glfw.cmake)

file(GLOB_RECURSE SRC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX src FILES ${SRC_SOURCES})

file(GLOB IMGUI_IMPL_SOURCES 
	${CMAKE_SOURCE_DIR}/3rdparty/imguipack/3rdparty/imgui_docking/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imguipack/3rdparty/imgui_docking/backends/imgui_impl_glfw.h
	${CMAKE_SOURCE_DIR}/3rdparty/imguipack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imguipack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.h
	${CMAKE_SOURCE_DIR}/3rdparty/imguipack/3rdparty/imgui_docking/misc/freetype/imgui_freetype.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/imguipack/3rdparty/imgui_docking/misc/freetype/imgui_freetype.h)
source_group(src/imgui FILES ${IMGUI_IMPL_SOURCES})

add_executable(${PROJECT}
	${SRC_SOURCES}
	${IMGUI_IMPL_SOURCES}
)

if(MSVC)
	set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
	set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS_DEBUG "/ENTRY:mainCRTStartup")
	set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup")
	set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS_MINSIZEREL "/ENTRY:mainCRTStartup")
	set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/ENTRY:mainCRTStartup")
endif()

target_include_directories(${PROJECT} PRIVATE  
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/headers
)

target_link_libraries(${PROJECT} PRIVATE imguipack glad glfw)

set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${FINAL_BIN_DIR})
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${FINAL_BIN_DIR})
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${FINAL_BIN_DIR})
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${FINAL_BIN_DIR})
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${FINAL_BIN_DIR})

set_target_properties(${PROJECT} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${FINAL_BIN_DIR}")

if(MSVC)
    set_property(TARGET ${PROJECT} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
