#include "Graphics/RenderTarget.h"
#include "Graphics/Texture2D.h"
#include "Graphics/API/GraphicsAPI.h"

class RenderTarget::Impl
{
public:

  Impl(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
  {
    for (unsigned int i = 0; i < _uNumColorAttachments; i++)
    {
      m_lstColorTextures.push_back(new Texture2D(_uWidth, _uHeight, _eFormat, 0, PipelineStage::PIXEL, _uMipLevels, _uMsaaSamples));
    }

    if (_bHasDepthStencil)
    {
      m_pDepthStencilTexture = new Texture2D(_uWidth, _uHeight, ImageFormat::D32, 0, PipelineStage::PIXEL, 1u, _uMsaaSamples);
    }

    if (_uMsaaSamples > 1u)
    {
      for (unsigned int i = 0; i < _uNumColorAttachments; i++)
      {
        m_lstColorResolveTextures.push_back(new Texture2D(_uWidth, _uHeight, _eFormat, 0, PipelineStage::PIXEL, 1u));
      }      
    }

    m_pAPIRenderTarget = api::CreateAPIRenderTarget();

    api::BeginRenderTargetSetup(m_pAPIRenderTarget);

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
 
};

RenderTarget::RenderTarget(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
{  
  m_pImpl = std::make_unique<Impl>(_uNumColorAttachments, _uWidth, _uHeight, _eFormat, _bHasDepthStencil, _uMipLevels, _uMsaaSamples);
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::Bind() const
{
  api::BindAPIRenderTarget(m_pImpl->m_pAPIRenderTarget);
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
