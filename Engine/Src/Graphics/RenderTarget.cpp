#include "Graphics/RenderTarget.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureUsage.h"
#include "Graphics/SamplerConfig.h"
#include "Memory/Factory.h"
#include "Core/Engine.h"

void RenderTarget::Configure(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels, unsigned int _uMsaaSamples, bool _bIsCubemap)
{  

  static const char s_aNames[4][7] = { "Input0", "Input1", "Input2", "Input3" };

  m_uMsaaSamples = _uMsaaSamples;

  uint32_t uColorUsage = TextureUsage::COLOR_TARGET;
  if (_uMsaaSamples <= 1u)
  {
    uColorUsage |= TextureUsage::SHADER_RESOURCE;
  }

  SamplerConfig oSamplerConfig = {};
  oSamplerConfig.m_eAddressMode = TextureAddressMode::MIRRORED_REPEAT;
  oSamplerConfig.m_eMipmapFilterMode = TextureFilterMode::LINEAR;
  oSamplerConfig.m_eMinFilterMode = TextureFilterMode::LINEAR;
  oSamplerConfig.m_eMagFilterMode = TextureFilterMode::LINEAR;

  for (unsigned int i = 0; i < _uNumColorAttachments; i++)
  {
    m_lstColorTextures.push_back(Factory::Create<Texture2D>());
    if (_bIsCubemap)
    {
      m_lstColorTextures.back()->ConfigureAsCubemap(nullptr, _uWidth, _uHeight, _eFormat, oSamplerConfig, _uMipLevels, _uMsaaSamples, uColorUsage);
    }
    else
    {
      m_lstColorTextures.back()->Configure(_uWidth, _uHeight, _eFormat, oSamplerConfig, _uMipLevels, _uMsaaSamples, uColorUsage);
    }
  }

  if (_bHasDepthStencil)
  {
    m_pDepthStencilTexture = Factory::Create<Texture2D>();
    if (_bIsCubemap)
    {
      m_pDepthStencilTexture->ConfigureAsCubemap(nullptr, _uWidth, _uHeight, ImageFormat::R32, oSamplerConfig, 1u, _uMsaaSamples, TextureUsage::SHADER_RESOURCE | TextureUsage::DEPTH_TARGET);
    }
    else
    {
      m_pDepthStencilTexture->Configure(_uWidth, _uHeight, ImageFormat::R32, oSamplerConfig, 1u, _uMsaaSamples, TextureUsage::SHADER_RESOURCE | TextureUsage::DEPTH_TARGET);
    }
  }

  if (_uMsaaSamples > 1u)
  {
    for (unsigned int i = 0; i < _uNumColorAttachments; i++)
    {      
      m_lstColorResolveTextures.push_back(Factory::Create<Texture2D>());
      if (_bIsCubemap)
      {
        m_lstColorResolveTextures.back()->ConfigureAsCubemap(nullptr, _uWidth, _uHeight, _eFormat, oSamplerConfig, 1u, TextureUsage::SHADER_RESOURCE | TextureUsage::COLOR_TARGET);
      }
      else
      {
        m_lstColorResolveTextures.back()->Configure(_uWidth, _uHeight, _eFormat, oSamplerConfig, 1u, TextureUsage::SHADER_RESOURCE | TextureUsage::COLOR_TARGET);
      }
    }
  }

  m_pAPIRenderTarget = api::CreateAPIRenderTarget();

  api::BeginRenderTargetSetup(m_pAPIRenderTarget, _uWidth, _uHeight, _eFormat, ImageFormat::R32, _uMsaaSamples, _bIsCubemap);

  for (owner_ptr<Texture2D>& pTexture : m_lstColorTextures)
  {
    pTexture->SetupAsRenderTargetColor();
  }

  for (owner_ptr<Texture2D>& pTexture : m_lstColorResolveTextures)
  {
    pTexture->SetupAsRenderTargetColorResolve();
  }

  if (m_pDepthStencilTexture)
  {
    m_pDepthStencilTexture->SetupAsRenderTargetDepthStencil();
  }

  api::EndRenderTargetSetup(ENGINE_API_WINDOW);
}

RenderTarget::~RenderTarget()
{
  api::DestroyAPIRenderTarget(ENGINE_API_WINDOW, m_pAPIRenderTarget);
}

void RenderTarget::SetUsing() const
{
}

void RenderTarget::Bind() const
{
  api::BindAPIRenderTarget(ENGINE_API_WINDOW, m_pAPIRenderTarget);
}

void RenderTarget::Unbind() const
{
  api::UnbindAPIRenderTarget(ENGINE_API_WINDOW, m_pAPIRenderTarget);
}

void RenderTarget::Clear() const
{
  api::ClearAPIRenderTarget(ENGINE_API_WINDOW, m_pAPIRenderTarget);

  for (const owner_ptr<Texture2D>& pTexture : m_lstColorTextures)
  {
    pTexture->ClearAsColor();
  }

  for (const owner_ptr<Texture2D>& pTexture : m_lstColorResolveTextures)
  {
    pTexture->ClearAsColor();
  }

  if (m_pDepthStencilTexture)
  {
    m_pDepthStencilTexture->ClearAsDepthStencil();
  }
}

const std::vector<owner_ptr<Texture2D>>& RenderTarget::GetColorTextures() const
{
  if (!m_lstColorResolveTextures.empty())
  {
    return m_lstColorResolveTextures;
  }

  return m_lstColorTextures;
}

Texture2D* RenderTarget::GetDepthStencilTexture() const 
{
  return m_pDepthStencilTexture.get();
}

uint32_t RenderTarget::GetMsaaSamples() const
{
  return m_uMsaaSamples;
}
