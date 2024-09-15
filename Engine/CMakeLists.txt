cmake_minimum_required(VERSION 3.8)

project(Engine VERSION 0.0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME}
	"Src/Graphics/Window.cpp"
	"Include/Graphics/Window.h"	
	"Include/Core/Engine.h"
	"Src/Core/Engine.cpp" 	 
	"Include/Core/Singleton.h" 	
	"Include/Core/Timer.h" 
	"Src/Core/Timer.cpp" 
	"Include/Graphics/Renderer.h" 
	"Include/Graphics/API/Vulkan/VulkanAPI.h"
	"Src/Graphics/API/Vulkan/VulkanAPI.cpp" 
	"Include/Graphics/API/Vulkan/VulkanMacros.h" 
	"Include/Core/Exception.h" 
	"Src/Core/Exception.cpp" 
	"Include/Graphics/API/Vulkan/VulkanException.h" 
	"Src/Graphics/API/Vulkan/VulkanException.cpp" 
	"Include/Graphics/API/Vulkan/VulkanData.h" 
	"Src/Graphics/API/GraphicsAPI.cpp" 
	"Src/Graphics/Renderer.cpp" 
	"Include/Graphics/API/Vulkan/APIWindow.h"  
	"Include/File/FileUtils.h" 
	"Src/File/FileUtils.cpp"  
	"Include/File/File.h" 
	"Src/File/File.cpp" 
	"Include/Core/GlobalMacros.h" 	
	"Include/Graphics/Mesh.h" 
	"Include/Graphics/PODTypes.h"
	"Src/Graphics/Mesh.cpp" 
	"Include/Graphics/API/Vulkan/APIMesh.h" 
	"Include/Graphics/Vertex.h" 
	"Include/Core/InputManager.h"
	"Src/Core/InputManager.cpp" 
	"Include/Graphics/API/Vulkan/APIConstantBuffer.h" 
	"Include/Graphics/ConstantBuffer.h" 
	"Src/Graphics/ConstantBuffer.cpp" 
	"Include/Components/ModelComponent.h"
	"Include/Math/Transform.h"  
	"Include/Core/Entity.h" 
	"Include/Core/Component.h"
	"Include/Core/Scene.h" 
	"Src/Core/Scene.cpp"
	"Include/Math/Utils.h"
	"Src/Components/ModelComponent.cpp"
	"Src/Core/Entity.cpp"
	"Src/Math/Transform.cpp" 
	"Include/Graphics/Material.h"
	"Include/Graphics/Resource.h"
	"Src/Graphics/Material.cpp"
	"Include/Graphics/MaterialInstance.h"
	"Src/Graphics/MaterialInstance.cpp"
	"Include/Graphics/API/Vulkan/APIRenderState.h" 
	"Include/Core/ImageManager.h" 
	"Src/Core/ImageManager.cpp" 
	"Include/Graphics/Texture2D.h" 
	"Src/Graphics/Texture2D.cpp"
	"Include/Graphics/API/Vulkan/APITexture.h"
	"Include/Graphics/ImageFormat.h" 
	"Include/Graphics/API/Vulkan/DescriptorUtils.h" 
	"Src/Graphics/API/Vulkan/DescriptorUtils.cpp"
	"Include/Util/ModelLoader.h" 
	"Src/Util/ModelLoader.cpp" 
	"Include/Graphics/ResourceBindInfo.h" 
	"Include/Graphics/API/Vulkan/APIRenderSubState.h" 
	"Include/Graphics/ResourceFrequency.h"	
	"Include/Graphics/API/Vulkan/VulkanShaderReflection.h"
	"Src/Graphics/API/Vulkan/VulkanShaderReflection.cpp"
	"3rd/spirv-reflect/spirv_reflect.h"
	"3rd/spirv-reflect/spirv_reflect.cpp"
	"3rd/spirv-reflect/include/spirv/unified1/spirv.h" 
	"Include/Graphics/MaterialLibrary.h"
	"Include/Components/CameraComponent.h" 
	"Src/Components/CameraComponent.cpp" 
	"Include/Graphics/API/Vulkan/APICamera.h"
	"Include/Graphics/Camera.h"
	"Src/Graphics/Camera.cpp" 
	"Include/Util/KeyboardUtil.h" 
	"Src/Util/KeyboardUtil.cpp" 
	"Include/Graphics/RenderStateInfo.h"
	"Include/Graphics/API/DX11/DX11API.h"
	"Src/Graphics/API/DX11/DX11API.cpp" 
	"Include/Graphics/API/DX11/APIWindow.h" 
	"Include/Graphics/API/DX11/DX11Exception.h"
	"Src/Graphics/API/DX11/DX11Exception.cpp" 
	"Include/Graphics/API/DX11/DX11Macros.h"
	"3rd/dxerr/dxerr.h"
	"3rd/dxerr/dxerr.cpp" 
	"Include/Graphics/API/DX11/APIConstantBuffer.h" 
	"Include/Graphics/API/DX11/DX11Data.h"
	"Include/Graphics/API/DX11/APIRenderState.h" 
	"Include/Util/WideStringUtil.h"
	"Include/Graphics/API/DX11/APIMesh.h" 
	"Include/Graphics/API/DX11/APICamera.h" 
	"Include/Graphics/API/DX11/DX11BindSlotOffset.h" 
	"Include/Graphics/API/DX11/APITexture.h" 
	"Include/Graphics/API/DX11/APIRenderTarget.h" 
	"Include/Graphics/BlendEnums.h"  
	"Include/Graphics/RenderTarget.h" 
	"Include/Graphics/API/Vulkan/APIRenderTarget.h"
	"Include/Graphics/RenderPipeline.h" 
	"Src/Graphics/RenderTarget.cpp"
	"Include/Graphics/API/Vulkan/APIInternal.h"
	"Src/Graphics/API/Vulkan/APIInternal.cpp"
	"Include/Graphics/API/Vulkan/RenderTargetBuilder.h"
	"Src/Graphics/API/Vulkan/RenderTargetBuilder.cpp"
	"Include/Graphics/RenderStep.h"
	"Src/Graphics/RenderStep.cpp" "Src/Graphics/RenderPipeline.cpp" "Include/Graphics/RenderPipelineConfig.h" "Include/Graphics/Job.h" "Src/Graphics/Job.cpp")

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/Include"
	"$ENV{VULKAN_SDK}/Include"
	"${CMAKE_SOURCE_DIR}/3rd"
	"${CMAKE_SOURCE_DIR}/glfw/include"
	"${CMAKE_SOURCE_DIR}/glm"
	"${CMAKE_SOURCE_DIR}/stb"
	"${CMAKE_SOURCE_DIR}/assimp/include"	
 )

set(RENDER_API_VULKAN 0)
set(RENDER_API_DX11 1)

set(TARGET_RENDER_API ${RENDER_API_VULKAN})

target_compile_definitions(${PROJECT_NAME} PRIVATE 
	RENDER_API=${TARGET_RENDER_API}
	CURR_WORKING_DIR=${CMAKE_SOURCE_DIR})

 # Shader stuff

file(GLOB_RECURSE V_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Vertex/*.hlsl")
file(GLOB_RECURSE P_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Pixel/*.hlsl")  

set(SHADER_BINARY_FILES)

function(CompileHLSL FILE TYPE_ARG)
  get_filename_component(FILE_NAME ${FILE} NAME_WE)
  get_filename_component(FILE_PATH ${FILE} PATH)  
  set(CSO "${FILE_PATH}/${FILE_NAME}.cso")  
  set(API_ARG)
  if(${TARGET_RENDER_API} MATCHES ${RENDER_API_DX11})
    set(API_ARG dx11)
  else()
    set(API_ARG vk)
  endif()

  add_custom_command(OUTPUT ${CSO} 
                     COMMAND python3 ${CMAKE_SOURCE_DIR}/Tools/ShaderCompiler/ShaderCompiler.py ${TYPE_ARG} ${API_ARG} ${FILE}
                     DEPENDS ${FILE}
                     VERBATIM)  
  
  list(APPEND SHADER_BINARY_FILES ${CSO})
  set(SHADER_BINARY_FILES ${SHADER_BINARY_FILES} PARENT_SCOPE)  
endfunction()

foreach(FILE ${V_SHADER_FILES})  
CompileHLSL(${FILE} "v")
endforeach(FILE)

foreach(FILE ${P_SHADER_FILES})  
CompileHLSL(${FILE} "p")
endforeach(FILE)


add_custom_target(
    Shaders 
    ALL DEPENDS ${SHADER_BINARY_FILES}) 

add_dependencies(${PROJECT_NAME} Shaders)

set_target_properties(${PROJECT_NAME} PROPERTIES  
    CXX_STANDARD 17)