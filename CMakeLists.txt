cmake_minimum_required(VERSION 3.8)
project(RenderEngine VERSION 0.0.1 LANGUAGES CXX)

set(RENDER_API_VULKAN 0)
set(RENDER_API_DX11 1)

set(TARGET_RENDER_API ${RENDER_API_DX11})  

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(assimp)
add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(App)
