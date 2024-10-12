
#include "Graphics/Texture2D.h"
#include "Core/ImageManager.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/Material.h"
#include "Graphics/ResourceBindInfo.h"

class Texture2D::Impl
{
public:
  api::APITexture* m_pAPITexture;

  Impl(void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, unsigned int _uMipLevels, unsigned int _uMsaaSamples, uint32_t _uUsage)
  {    
    m_pAPITexture = api::CreateAPITexture(_pData, _uWidth, _uHeight, _eFormat, _uMipLevels, _uMsaaSamples, _uUsage);
  }

  ~Impl()
  {
    api::DestroyAPITexture(m_pAPITexture);
  }

};

Texture2D::Texture2D(const std::string& _sFilename, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
  : Resource(_iBinding, _eStage)
{
  const Image& rImage = ImageManager::GetInstance()->LoadImage(_sFilename, true);
  m_pImpl = std::make_unique<Impl>(rImage.m_pData, rImage.m_iWidth, rImage.m_iHeight, rImage.m_eFormat, _uMipLevels, _uMsaaSamples, TextureUsage::SHADER_RESOURCE);
}

Texture2D::Texture2D(const Image& _rImage, int _iBinding , PipelineStage _eStage, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
  : Resource(_iBinding, _eStage)
{  
  m_pImpl = std::make_unique<Impl>(_rImage.m_pData, _rImage.m_iWidth, _rImage.m_iHeight, _rImage.m_eFormat, _uMipLevels, _uMsaaSamples, TextureUsage::SHADER_RESOURCE);
}

Texture2D::Texture2D(uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels, unsigned int _uMsaaSamples, uint32_t _uUsage)
  : Resource(_iBinding, _eStage)
{
  m_pImpl = std::make_unique<Impl>(nullptr, _uWidth, _uHeight, _eFormat, _uMipLevels, _uMsaaSamples, _uUsage);
}

void Texture2D::SetupRenderSubState(ResourceFrequency _eFrequency) const
{  
  ResourceBindInfo oBindInfo {};
  oBindInfo.m_eLevel = _eFrequency;
  oBindInfo.m_eStage = m_eStage;
  oBindInfo.m_iBinding = m_iBinding;
  api::SubStateSetupTexture(m_pImpl->m_pAPITexture, oBindInfo);
}

void Texture2D::SetupAsRenderTargetColor() const
{
  api::RenderTargetAddColorTexture(m_pImpl->m_pAPITexture);
}

void Texture2D::SetupAsRenderTargetDepthStencil() const
{
  api::RenderTargetSetDepthStencilTexture(m_pImpl->m_pAPITexture);
}

void Texture2D::SetupAsRenderTargetColorResolve() const
{
  api::RenderTargetAddColorResolveTexture(m_pImpl->m_pAPITexture);
}

void Texture2D::Bind() const
{
  api::BindAPITexture(m_pImpl->m_pAPITexture);
}