cmake_minimum_required(VERSION 3.8)

project(App VERSION 0.0.1 LANGUAGES CXX)

set(
	source_list
	"Src/main.cpp"
)

add_executable(${PROJECT_NAME} ${source_list} ${VERTEXSHADER_FILES} ${PIXELSHADER_FILES})

foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/Engine/Include)
include_directories(${CMAKE_SOURCE_DIR}/glfw/include)
include_directories(${CMAKE_SOURCE_DIR}/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd)

link_directories($ENV{VULKAN_SDK}/Bin;$ENV{VULKAN_SDK}/Lib;)

find_package(Vulkan REQUIRED)
target_link_libraries(App Vulkan::Vulkan)
target_link_libraries(App d3d11.lib)
target_link_libraries(App dxgi.lib)
target_link_libraries(App d3dcompiler.lib)
target_link_libraries(App dxguid.lib)
target_link_libraries(App Engine)
target_link_libraries(App glfw ${GLFW_LIBRARIES})
target_link_libraries(App glm::glm)
target_link_libraries(App assimp::assimp)

set_target_properties(${PROJECT_NAME} PROPERTIES  
    CXX_STANDARD 17
    # VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
