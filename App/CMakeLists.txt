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

link_directories($ENV{VULKAN_SDK}/Bin;$ENV{VULKAN_SDK}/Lib;)

find_package(Vulkan REQUIRED)
target_link_libraries(App Vulkan::Vulkan)
target_link_libraries(App Engine)
target_link_libraries(App glfw ${GLFW_LIBRARIES})

# Shader stuff

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_COMPILER "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
  set(GLSL_COMPILER "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
endif()

file(GLOB_RECURSE SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/*")

foreach(FILE ${SHADER_FILES})  
  get_filename_component(FILE_NAME ${FILE} NAME_WE)  
  get_filename_component(FILE_PATH ${FILE} PATH)  
  file(RELATIVE_PATH RELATIVE_FILE ${CMAKE_SOURCE_DIR} ${FILE})
  get_filename_component(RELATIVE_PATH ${RELATIVE_FILE} PATH)  
  set(BINARY_PATH "${CMAKE_BINARY_DIR}/${RELATIVE_PATH}/")
  set(SPIRV "${BINARY_PATH}${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BINARY_PATH}
    COMMAND ${GLSL_COMPILER} ${FILE} -o ${SPIRV}
    DEPENDS ${FILE})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
  message(STATUS "compiling shader: ${FILE}")
  message(STATUS "output file: ${SPIRV}")
endforeach(FILE)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(${PROJECT_NAME} Shaders)

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/"
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${PROJECT_BINARY_DIR}/shaders"
#        "$<TARGET_FILE_DIR:YourMainTarget>/shaders"
#        )

set_target_properties(${PROJECT_NAME} PROPERTIES  
    CXX_STANDARD 17
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
