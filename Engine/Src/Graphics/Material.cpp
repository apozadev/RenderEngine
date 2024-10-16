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


Material::Material(std::vector<Pass>&& _lstPasses)
  : m_lstPasses(std::move(_lstPasses))
{   
}

Material::Material(Material&& rMaterial)
  : m_lstPasses(std::move(rMaterial.m_lstPasses))
{
}

void Material::Setup() const
{
  for (const Pass& rPass : m_lstPasses)
  {
    rPass.Setup();
  }
}
