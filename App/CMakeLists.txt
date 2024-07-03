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
target_link_libraries(App Engine)
target_link_libraries(App glfw ${GLFW_LIBRARIES})
target_link_libraries(App glm::glm)
target_link_libraries(App assimp::assimp)

# Shader stuff

# if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_COMPILER "$ENV{VULKAN_SDK}/Bin/glslc.exe")
# else()
#   set(GLSL_COMPILER "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
# endif()

#set(HLSL_COMPILER "dxc")
set(HLSL_COMPILER "fxc")

file(GLOB_RECURSE VK_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Vulkan/*.glsl")

set(SPIRV_BINARY_FILES)

foreach(FILE ${VK_SHADER_FILES})  
  # get_filename_component(FILE_NAME ${FILE} NAME_WE)  
  # get_filename_component(FILE_PATH ${FILE} PATH)  
  # file(RELATIVE_PATH RELATIVE_FILE ${CMAKE_SOURCE_DIR} ${FILE})
  # get_filename_component(RELATIVE_PATH ${RELATIVE_FILE} PATH)  
  # set(BINARY_PATH "${CMAKE_BINARY_DIR}/${RELATIVE_PATH}/")
  # set(SPIRV "${BINARY_PATH}${FILE_NAME}.spv")

  get_filename_component(FILE_NAME ${FILE} NAME_WE)
  get_filename_component(FILE_PATH ${FILE} PATH)  
  set(SPIRV "${FILE_PATH}/${FILE_NAME}.spv") 
   
  add_custom_command(
    OUTPUT ${SPIRV}
    # COMMAND ${CMAKE_COMMAND} -E make_directory ${BINARY_PATH}
    COMMAND ${GLSL_COMPILER} ${FILE} -o ${SPIRV} 
    DEPENDS ${FILE}
    COMMENT "compiling shader: ${FILE} \n output file: ${SPIRV}")
  set(SPIRV_BINARY_FILES ${SPIRV_BINARY_FILES} ${SPIRV})
endforeach(FILE)

file(GLOB_RECURSE DX11_V_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/DX11/Vertex/*.hlsl")
file(GLOB_RECURSE DX11_P_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/DX11/Pixel/*.hlsl")

set(CSO_BINARY_FILES)

function(CompileHLSL FILE TYPE_ARG)
  get_filename_component(FILE_NAME ${FILE} NAME_WE)
  get_filename_component(FILE_PATH ${FILE} PATH)  
  set(CSO "${FILE_PATH}/${FILE_NAME}.cso") 

  add_custom_command(OUTPUT ${CSO} 
                     COMMAND ${HLSL_COMPILER} -E main -T ${TYPE_ARG} -Fo ${CSO} ${FILE}
                     DEPENDS ${FILE}
                     COMMENT "compiling shader: ${FILE} \n output file: ${CSO}"
                     VERBATIM)  
  
  list(APPEND CSO_BINARY_FILES ${CSO})
  set(CSO_BINARY_FILES ${CSO_BINARY_FILES} PARENT_SCOPE)  
endfunction()

foreach(FILE ${DX11_V_SHADER_FILES})  
  CompileHLSL(${FILE} vs_5_0)
endforeach(FILE)

foreach(FILE ${DX11_P_SHADER_FILES})  
  CompileHLSL(${FILE} ps_5_0)
endforeach(FILE)

add_custom_target(
    Shaders 
    ALL DEPENDS ${CSO_BINARY_FILES} ${SPIRV_BINARY_FILES}) 

add_dependencies(${PROJECT_NAME} Shaders)

set_target_properties(${PROJECT_NAME} PROPERTIES  
    CXX_STANDARD 17
    # VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
