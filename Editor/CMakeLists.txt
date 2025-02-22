cmake_minimum_required(VERSION 3.8)

project(Editor VERSION 0.0.1 LANGUAGES CXX)

set(
	source_list
	"main.cpp"
    "SceneEditor.h" 
    "SceneEditor.cpp")

add_executable(${PROJECT_NAME} ${source_list})

foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/Engine/Include ${CMAKE_SOURCE_DIR}/Editor ${CMAKE_SOURCE_DIR}/Editor/3rd)

link_directories($ENV{VULKAN_SDK}/Bin;$ENV{VULKAN_SDK}/Lib;)

find_package(Vulkan REQUIRED)
target_link_libraries(Editor PRIVATE Vulkan::Vulkan)
target_link_libraries(Editor PRIVATE d3d11.lib)
target_link_libraries(Editor PRIVATE dxgi.lib)
target_link_libraries(Editor PRIVATE d3dcompiler.lib)
target_link_libraries(Editor PRIVATE dxguid.lib)
#target_link_options(Editor PRIVATE -Wl,--whole-archive)
#target_link_libraries(Editor PRIVATE Engine)
#target_link_options(Editor PRIVATE -Wl,--no-whole-archive)
target_link_libraries(Editor PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,Engine>")
target_link_libraries(Editor PRIVATE glfw ${GLFW_LIBRARIES})
target_link_libraries(Editor PRIVATE glm::glm)
target_link_libraries(Editor PRIVATE assimp::assimp)

if(${TARGET_RENDER_API} MATCHES ${RENDER_API_VULKAN})
	target_compile_definitions(${PROJECT_NAME} PRIVATE RENDER_API_VULKAN) 
else()
target_compile_definitions(${PROJECT_NAME} PRIVATE RENDER_API_DX11) 
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES  
    CXX_STANDARD 17
    # VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}") 
