#include "Graphics/MaterialInstance.h"
#include "Graphics/Resource.h"
#include "Graphics/ResourceBindInfo.h"
#include "Core/Exception.h"

namespace matinstance_internal
{
  static uint16_t s_uNextId = 0u;
}

void MaterialInstance::Setup(Material* _pMaterial)
{    

  static const char s_aNames[4][9] = { "Texture0", "Texture1", "Texture2", "Texture3" };

  if (m_bSetup)
  {
    return;
  }

  m_pMaterial = _pMaterial;

  m_uId = matinstance_internal::s_uNextId++;
  m_pSubState = api::CreateAPIRenderSubState(ResourceFrequency::MATERIAL_INSTANCE);

  api::BeginSubStateSetup(m_pSubState);

  for (int i = 0; i < m_lstTextures.size(); i++)
  {     
    const owner_ptr<Texture2D>& pTexture = m_lstTextures[i];
    pTexture->SetupRenderSubState(s_aNames[i], PipelineStage::PIXEL, ResourceFrequency::MATERIAL_INSTANCE);
  }

  /*for (const ConstantBufferBase* pCBuffer : m_lstCBuffers)
  {
    pCBuffer->SetupRenderSubState(ResourceFrequency::MATERIAL_INSTANCE);
  }*/

  api::EndSubStateSetup(ResourceFrequency::MATERIAL_INSTANCE);
  
  m_bSetup = true;
}

void MaterialInstance::Bind() const
{  
  if (!m_bSetup)
  {
    THROW_GENERIC_EXCEPTION("Material Instance was not set up")
  }  

  api::BindAPIRenderSubState(m_pSubState, ResourceFrequency::MATERIAL_INSTANCE);

  for (const owner_ptr<Texture2D>& pTexture : m_lstTextures)
  {
    pTexture->Bind();
  }

  /*for (const ConstantBufferBase* pCBuffer : m_lstCBuffers)
  {
    pCBuffer->Bind();
  }*/
}
