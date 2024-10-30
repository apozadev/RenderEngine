#include "Graphics/RenderTarget.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureUsage.h"
#include "Graphics/API/GraphicsAPI.h"

class RenderTarget::Impl
{
public:

  Impl(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
    : m_uMsaaSamples(_uMsaaSamples)
  {

    uint32_t uColorUsage = TextureUsage::COLOR_TARGET;
    if (_uMsaaSamples <= 1u)
    {
      uColorUsage |= TextureUsage::SHADER_RESOURCE;
    }

    for (unsigned int i = 0; i < _uNumColorAttachments; i++)
    {
      m_lstColorTextures.push_back(new Texture2D(_uWidth, _uHeight, _eFormat, 0, PipelineStage::PIXEL, _uMipLevels, _uMsaaSamples, uColorUsage));
    }

    if (_bHasDepthStencil)
    {
      m_pDepthStencilTexture = new Texture2D(_uWidth, _uHeight, ImageFormat::R32, 0, PipelineStage::PIXEL, 1u, _uMsaaSamples, TextureUsage::SHADER_RESOURCE | TextureUsage::DEPTH_TARGET);
    }

    if (_uMsaaSamples > 1u)
    {
      for (unsigned int i = 0; i < _uNumColorAttachments; i++)
      {
        m_lstColorResolveTextures.push_back(new Texture2D(_uWidth, _uHeight, _eFormat, 0, PipelineStage::PIXEL, 1u, TextureUsage::SHADER_RESOURCE | TextureUsage::COLOR_TARGET));
      }      
    }

    m_pAPIRenderTarget = api::CreateAPIRenderTarget();

    api::BeginRenderTargetSetup(m_pAPIRenderTarget, _eFormat, ImageFormat::R32, _uMsaaSamples);    

    for (Texture2D* pTexture : m_lstColorTextures)
    {
      pTexture->SetupAsRenderTargetColor();
    }

    for (Texture2D* pTexture : m_lstColorResolveTextures)
    {
      pTexture->SetupAsRenderTargetColorResolve();
    }

    m_pDepthStencilTexture->SetupAsRenderTargetDepthStencil();

    api::EndRenderTargetSetup();

  }

  ~Impl()
  {
    api::DestroyAPIRenderTarget(m_pAPIRenderTarget);

    for (Texture2D* pTexture : m_lstColorTextures)
    {
      delete pTexture;
    }

    for (Texture2D* pTexture : m_lstColorResolveTextures)
    {
      delete pTexture;
    }

    if (m_pDepthStencilTexture)
    {
      delete m_pDepthStencilTexture;
    }    
  }

  api::APIRenderTarget* m_pAPIRenderTarget;

  std::vector<Texture2D*> m_lstColorTextures;

  std::vector<Texture2D*> m_lstColorResolveTextures;

  Texture2D* m_pDepthStencilTexture;    

  uint32_t m_uMsaaSamples;
 
};

RenderTarget::RenderTarget(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
{  
  m_pImpl = std::make_unique<Impl>(_uNumColorAttachments, _uWidth, _uHeight, _eFormat, _bHasDepthStencil, _uMipLevels, _uMsaaSamples);
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::SetUsing() const
{
  api::SetUsingAPIRenderTarget(m_pImpl->m_pAPIRenderTarget);
}

void RenderTarget::Bind() const
{
  api::BindAPIRenderTarget(m_pImpl->m_pAPIRenderTarget);
}

void RenderTarget::Unbind() const
{
  api::UnbindAPIRenderTarget(m_pImpl->m_pAPIRenderTarget);
}

void RenderTarget::Clear() const
{
  api::ClearAPIRenderTarget(m_pImpl->m_pAPIRenderTarget);

  for (Texture2D* pTexture : m_pImpl->m_lstColorTextures)
  {
    pTexture->ClearAsColor();
  }

  for (Texture2D* pTexture : m_pImpl->m_lstColorResolveTextures)
  {
    pTexture->ClearAsColor();
  }

  m_pImpl->m_pDepthStencilTexture->ClearAsDepthStencil();
}

std::vector<Texture2D*> RenderTarget::GetColorTextures()
{
  if (!m_pImpl->m_lstColorResolveTextures.empty())
  {
    return m_pImpl->m_lstColorResolveTextures;
  }

  return m_pImpl->m_lstColorTextures;
}

Texture2D* RenderTarget::GetDepthStencilTexture()
{
  return m_pImpl->m_pDepthStencilTexture;
}

uint32_t RenderTarget::GetMsaaSamples() const
{
  return m_pImpl->m_uMsaaSamples;
}
