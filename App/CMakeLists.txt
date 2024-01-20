cmake_minimum_required(VERSION 3.8)

project(App VERSION 0.0.1 LANGUAGES CXX)

file(GLOB_RECURSE VERTEXSHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Vertex/*.hlsl*")
file(GLOB_RECURSE PIXELSHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Pixel/*.hlsl*")

set_source_files_properties(VERTEXSHADER_FILES PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT main)
set_source_files_properties(PIXELSHADER_FILES PROPERTIES VS_SHADER_TYPE Pixel  VS_SHADER_MODEL 5.0 VS_SHADER_ENTRYPOINT main)

set_property(SOURCE ${PIXELSHADER_FILES} PROPERTY VS_SHADER_TYPE Pixel)
set_property(SOURCE ${PIXELSHADER_FILES} PROPERTY VS_SHADER_ENTRYPOINT mainPS)
set_property(SOURCE ${PIXELSHADER_FILES} PROPERTY VS_SHADER_MODEL 5_0)
set_property(SOURCE ${PIXELSHADER_FILES} PROPERTY VS_SHADER_FLAGS $<1:/DFLAGS_ADDED>)
set_property(SOURCE ${PIXELSHADER_FILES} PROPERTY VS_SHADER_OUTPUT_HEADER_FILE "$(OutDir)%(Filename).h")

set_property(SOURCE ${VERTEXSHADER_FILES} PROPERTY VS_SHADER_TYPE Vertex)
set_property(SOURCE ${VERTEXSHADER_FILES} PROPERTY VS_SHADER_ENTRYPOINT mainVS)
set_property(SOURCE ${VERTEXSHADER_FILES} PROPERTY VS_SHADER_MODEL 5_0)
set_property(SOURCE ${VERTEXSHADER_FILES} PROPERTY VS_SHADER_FLAGS $<1:/DFLAGS_ADDED>)
set_property(SOURCE ${VERTEXSHADER_FILES} PROPERTY VS_SHADER_OUTPUT_HEADER_FILE "$(OutDir)%(Filename).h")
set_property(SOURCE ${VERTEXSHADER_FILES} PROPERTY VS_SETTINGS "$<$<CONFIG:DEBUG>:SourceProperty1=SourceProperty1Value>")

source_group("Shaders/Vertex" FILES ${VERTEXSHADER_FILES})
source_group("Shaders/Pixel" FILES ${PIXELSHADER_FILES})

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

target_link_libraries(App Engine)

set_target_properties(${PROJECT_NAME} PROPERTIES  
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Assets")