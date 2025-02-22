#include "Graphics/Material.h"
#include "Graphics/Resource.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Window.h"
#include "Graphics/Mesh.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/RenderStep.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Pass.h"
#include "Core/Exception.h"

#include "Reflection/ReflectionImplMacros.h"

void Material::AddPass(owner_ptr<Pass>&& _pPass)
{
  m_lstPasses.push_back(std::move(_pPass));
}

Material::~Material()
{
}

void Material::Setup() const
{
  for (const owner_ptr<Pass>& pPass : m_lstPasses)
  {
    pPass->Setup();
  }
}

REFLECT_STRUCT_BASE_BEGIN(Material)
REFLECT_STRUCT_MEMBER(m_lstPasses)
REFLECT_STRUCT_END(Material)
