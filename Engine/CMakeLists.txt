cmake_minimum_required(VERSION 3.8)

project(Engine VERSION 0.0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME}
	"Src/core.cpp"
	"Include/core.h"
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/Include"
 )