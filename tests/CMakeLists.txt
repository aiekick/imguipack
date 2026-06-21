cmake_minimum_required(VERSION 3.10)

set(PROJECT imguipack_tests)
project(${PROJECT} CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_definitions(-DNOMINMAX)
endif()

file(GLOB_RECURSE PROJECT_TEST_SRC_RECURSE 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Src FILES ${PROJECT_TEST_SRC_RECURSE})

add_executable(${PROJECT} 
	${SRC_RECURSE}
	${PROJECT_TEST_SRC_RECURSE}
)

set_target_properties(${PROJECT} PROPERTIES FOLDER Tests)

target_include_directories(${PROJECT} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT} PRIVATE imguipack)

if(MSVC)
	set_property(TARGET ${PROJECT} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

function(AddTest arg)
  	add_test("${arg}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}" "${arg}")
endfunction(AddTest)

##########################################################
## TESTS #################################################
##########################################################

AddTest(TestImCode_TextRoundTrip)
AddTest(TestImCode_Insert)
AddTest(TestImCode_BackspaceDelete)
AddTest(TestImCode_Selection)
AddTest(TestImCode_UndoRedo)
AddTest(TestImCode_Indent)
