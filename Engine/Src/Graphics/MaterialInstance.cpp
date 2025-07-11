#include "Graphics/MaterialInstance.h"
#include "Graphics/Resource.h"
#include "Graphics/ResourceBindInfo.h"
#include "Core/Exception.h"
#include "Core/Engine.h"

#include "Reflection/ReflectionImplMacros.h"

namespace matinstance_internal
{
  static uint16_t s_uNextId = 0u;
}

void MaterialInstance::SetupSubState(const Material* _pMaterial)
{    

  static const char s_aNames[4][9] = { "Texture0", "Texture1", "Texture2", "Texture3" };
  
  if (m_bSetup)
  {
    return;
  }

  m_pMaterial = _pMaterial;

  m_uId = matinstance_internal::s_uNextId++;
  m_pSubState = api::CreateAPIRenderSubState(ENGINE_API_WINDOW, ResourceFrequency::MATERIAL_INSTANCE);

  api::BeginSubStateSetup(ENGINE_API_WINDOW, m_pSubState, ResourceFrequency::MATERIAL_INSTANCE);

  for (int i = 0; i < 4 && i < m_lstTextures.size(); i++)
  { 
    ResourceBindInfo oBindInfo{};
    oBindInfo.m_eLevel = ResourceFrequency::MATERIAL_INSTANCE;
    oBindInfo.m_uStageFlags = STAGE_PIXEL;
    oBindInfo.m_sName = s_aNames[i];
    api::SubStateSetupTexture(ENGINE_API_WINDOW, m_lstTextures[i]->m_pAPITexture, oBindInfo);
  }

  /*for (const ConstantBufferBase* pCBuffer : m_lstCBuffers)
  {
    pCBuffer->SetupRenderSubState(ResourceFrequency::MATERIAL_INSTANCE);
  }*/

  api::EndSubStateSetup(ENGINE_API_WINDOW);
  
  m_bSetup = true;
}

void MaterialInstance::Bind(const Pass* _pPass) const
{  
  if (!m_bSetup)
  {
    THROW_GENERIC_EXCEPTION("Material Instance was not set up")
  }  

  api::BindAPIRenderSubState(ENGINE_API_WINDOW, _pPass->m_pAPIRenderState, m_pSubState, ResourceFrequency::MATERIAL_INSTANCE);

  for (const owner_ptr<Texture2D>& pTexture : m_lstTextures)
  {
    pTexture->Bind();
  }

  /*for (const ConstantBufferBase* pCBuffer : m_lstCBuffers)
  {
    pCBuffer->Bind();
  }*/
}

REFLECT_STRUCT_BASE_BEGIN(MaterialInstance)
//REFLECT_STRUCT_MEMBER(m_lstTextures)
REFLECT_STRUCT_END(MaterialInstance)

IMPLEMENT_REFLECTION_POINTER(MaterialInstance)

IMPLEMENT_REFLECTION_VECTOR(owner_ptr<MaterialInstance>)
