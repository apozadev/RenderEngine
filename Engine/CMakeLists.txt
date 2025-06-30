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
	"Include/File/InFile.h" 
	"Src/File/InFile.cpp" 
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
	"Include/Graphics/MaterialLibrary.h"
	"Include/Components/CameraComponent.h" 
	"Src/Components/CameraComponent.cpp" 
	
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
	"Src/Graphics/BlendEnums.cpp"
	"Include/Graphics/RenderTarget.h" 
	"Include/Graphics/API/Vulkan/APIRenderTarget.h"
	"Include/Graphics/RenderPipeline.h" 
	"Src/Graphics/RenderTarget.cpp"
	"Include/Graphics/API/Vulkan/APIInternal.h"
	"Src/Graphics/API/Vulkan/APIInternal.cpp"
	"Include/Graphics/API/Vulkan/RenderTargetBuilder.h"
	"Src/Graphics/API/Vulkan/RenderTargetBuilder.cpp"
	"Include/Graphics/RenderStep.h"
	"Src/Graphics/RenderStep.cpp" 
	"Src/Graphics/RenderPipeline.cpp" 
	"Src/Graphics/ImageFormat.cpp"
	"Include/Graphics/RenderPipelineConfig.h" 
	"Src/Graphics/RenderPipelineConfig.cpp"
	"Include/Graphics/Job.h" 
	"Src/Graphics/Job.cpp" 
	"Include/Graphics/API/DX11/RenderTargetBuilder.h" 
	"Include/Graphics/TextureUsage.h" 
	"Include/Graphics/Pass.h" 
	"Src/Graphics/Pass.cpp" 
	"Include/Graphics/RenderKey.h" 
	"Src/Core/Singleton.cpp"
	"Include/Graphics/API/DX11/APIInternal.h"
	"Src/Graphics/API/DX11/APIInternal.cpp" 
	"Src/Graphics/API/DX11/RenderTargetBuilder.cpp" 
	"Include/Graphics/API/DX11/DX11Pools.h"
	"Include/Memory/TypedPool.h"
	"Src/Graphics/API/DX11/DX11Pools.cpp" 
	"Include/Graphics/API/Vulkan/VulkanPools.h" 
	"Src/Graphics/API/Vulkan/VulkanPools.cpp" 
	"Include/Memory/TypedContiguousPool.h" 
	"Include/Memory/PooledObject.h" 	
	"Include/Memory/GenericPool.h" 
	"Src/Memory/GenericPool.cpp" 
	"Include/Memory/TypedFactory.h"
	"Include/Memory/TemplateFactory.h"
	"Include/Memory/PoolObjectDeleter.h" 
	"Include/Memory/PtrTypes.h"
	"Include/Core/BaseObject.h"
	"Include/Reflection/TypeDescriptors.h"
	"Include/Reflection/TypeVisitor.h"
	"Include/Reflection/CopyTypeVisitor.h"
	"Include/Reflection/DeserializationTypeVisitor.h"
	"Include/Reflection/SerializationTypeVisitor.h"
	"Include/Reflection/UnloadTypeVisitor.h"
	"Include/Reflection/ReflectionHelper.h"
	"Include/Reflection/ReflectionImplMacros.h"
	"Include/Reflection/ReflectionMacros.h"
	"Include/Reflection/ReflectionSettings.h"	
	"Include/Reflection/ReflectionPrimitives.h"	
	"Src/Reflection/TypeDescriptors.cpp"	
	"Src/Reflection/CopyTypeVisitor.cpp"
	"Src/Reflection/DeserializationTypeVisitor.cpp"
	"Src/Reflection/SerializationTypeVisitor.cpp"
	"Src/Reflection/UnloadTypeVisitor.cpp"
	"Src/Reflection/ReflectionHelper.cpp"	
	"Src/Reflection/ReflectionSettings.cpp"
	"Src/Reflection/ReflectionPrimitives.cpp"
	"3rd/rapidxml/rapidxml.hpp"
	"3rd/rapidxml/rapidxml_ext.hpp"
	"Include/Memory/Factory.h" 
	"Include/Components/DirLightComponent.h" 
	"Src/Components/DirLightComponent.cpp" 
	"Include/Graphics/DirLight.h" 
	"Include/Graphics/GeometryRenderStep.h"
	"Src/Graphics/GeometryRenderStep.cpp"
	"Include/Graphics/ConstantBufferTypes.h" 
	"Include/Graphics/ReflectedConstantBuffer.h"
	"Src/Graphics/ReflectedConstantBuffer.cpp" 
	"Src/Graphics/ConstantBufferTypes.cpp"
	"Src/Core/Component.cpp"
	"Include/Core/SceneLoading.h"
	"Src/Core/SceneLoading.cpp"
	"Include/Core/SceneDesc.h" 
	"Include/Core/ComponentDesc.h"
	"Src/Core/ComponentDesc.cpp"
	"Src/Core/SceneDesc.cpp" 
	"Src/Graphics/RenderStateInfo.cpp" 
	"Src/Graphics/MaterialLibrary.cpp" 
	"Include/Graphics/SamplerConfig.h"
	"Include/Math/Vec3.h" 
	"Src/Math/Vec3.cpp" 
	"Include/Reflection/ImGuiTypeVisitor.h" 
	"Include/Graphics/CullMode.h" 
	"Src/Graphics/CullMode.cpp" 
	"Include/Graphics/DepthCompareOp.h" 
	"Src/Graphics/DepthCompareOp.cpp"
	"Src/Reflection/ImGuiTypeVisitor.cpp"
	"3rd/spirv-reflect/spirv_reflect.h"
	"3rd/spirv-reflect/spirv_reflect.cpp"
	"3rd/spirv-reflect/include/spirv/unified1/spirv.h" 
	"../imgui/imgui.h"
    "../imgui/imgui.cpp"
    "../imgui/imgui_internal.h"    
    "../imgui/imgui_draw.cpp"    
    "../imgui/imgui_tables.cpp"    
    "../imgui/imgui_widgets.cpp"    
    "../imgui/imstb_rectpack.h"    
    "../imgui/imstb_textedit.h"    
    "../imgui/imstb_truetype.h"    
    "../imgui/backends/imgui_impl_vulkan.h"
    "../imgui/backends/imgui_impl_vulkan.cpp"
    "../imgui/backends/imgui_impl_win32.h"
    "../imgui/backends/imgui_impl_win32.cpp"
	"../imgui/backends/imgui_impl_dx11.h"
	"../imgui/backends/imgui_impl_dx11.cpp"
    "../imgui/backends/imgui_impl_glfw.h"
    "../imgui/backends/imgui_impl_glfw.cpp"  "Include/Graphics/CBufferDataTypes.h" "Include/Graphics/PipelineLayoutConfig.h" "Include/Graphics/TextureSubResource.h") 

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/Include"
	"$ENV{VULKAN_SDK}/Include"
	"${CMAKE_CURRENT_LIST_DIR}/3rd"
	"${CMAKE_SOURCE_DIR}/glfw/include"
	"${CMAKE_SOURCE_DIR}/glm"
	"${CMAKE_SOURCE_DIR}/stb"
	"${CMAKE_SOURCE_DIR}/imgui"
	"${CMAKE_SOURCE_DIR}/assimp/include"	
 )

option(RENDER_API_VULKAN "Use Vulkan API" OFF)
option(RENDER_API_DX11 "Use DirectX 11 API" OFF)

# Define preprocessor macros based on the selected API
if(RENDER_API_VULKAN)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RENDER_API_VULKAN)
endif()

if(RENDER_API_DX11)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RENDER_API_DX11)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE CURR_WORKING_DIR=${CMAKE_SOURCE_DIR}) 

 # Shader stuff

file(GLOB_RECURSE V_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Vertex/*.vs")
file(GLOB_RECURSE P_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Pixel/*.ps")  
file(GLOB_RECURSE G_SHADER_FILES "${CMAKE_SOURCE_DIR}/Assets/Shaders/Geometry/*.gs")  

set(SHADER_BINARY_FILES)

function(CompileHLSL FILE TYPE_ARG)
  get_filename_component(FILE_NAME ${FILE} NAME_WE)
  get_filename_component(FILE_PATH ${FILE} PATH)    
  set(API_ARG)
  if(RENDER_API_DX11)    
	set(CSO "${FILE_PATH}/${FILE_NAME}.cso")    
	add_custom_command(OUTPUT ${CSO} 
                     COMMAND python3 ${CMAKE_SOURCE_DIR}/Tools/ShaderCompiler/ShaderCompiler.py ${TYPE_ARG} dx11 ${FILE}
                     DEPENDS ${FILE}
                     VERBATIM)  
	list(APPEND SHADER_BINARY_FILES ${CSO})
  else()    
	set(SPV "${FILE_PATH}/${FILE_NAME}.spv")  
	add_custom_command(OUTPUT ${SPV} 
                     COMMAND python3 ${CMAKE_SOURCE_DIR}/Tools/ShaderCompiler/ShaderCompiler.py ${TYPE_ARG} vk ${FILE}
                     DEPENDS ${FILE}
                     VERBATIM)  
	list(APPEND SHADER_BINARY_FILES ${SPV})
  endif()  
    
  set(SHADER_BINARY_FILES ${SHADER_BINARY_FILES} PARENT_SCOPE)  
endfunction()

foreach(FILE ${V_SHADER_FILES})  
CompileHLSL(${FILE} "v")
endforeach(FILE)

foreach(FILE ${G_SHADER_FILES})  
CompileHLSL(${FILE} "g")
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