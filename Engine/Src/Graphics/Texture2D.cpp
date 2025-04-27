
#include "Graphics/Texture2D.h"
#include "Core/ImageManager.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/Material.h"
#include "Graphics/ResourceBindInfo.h"


void Texture2D::Configure(const std::string& _sFilename, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
{
  const Image& rImage = ImageManager::GetInstance()->LoadImage(_sFilename, true);  
  m_pAPITexture = api::CreateAPITexture(&rImage.m_pData, rImage.m_iWidth, rImage.m_iHeight, rImage.m_eFormat, _uMipLevels, _uMsaaSamples, TextureUsage::SHADER_RESOURCE, _rSamplerConfig, false);

  m_iWidth = rImage.m_iWidth;
  m_iHeight = rImage.m_iHeight;
}

void Texture2D::Configure(const Image& _rImage, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels, unsigned int _uMsaaSamples)
{  
  m_pAPITexture = api::CreateAPITexture(&_rImage.m_pData, _rImage.m_iWidth, _rImage.m_iHeight, _rImage.m_eFormat, _uMipLevels, _uMsaaSamples, TextureUsage::SHADER_RESOURCE, _rSamplerConfig, false);

  m_iWidth = _rImage.m_iWidth;
  m_iHeight = _rImage.m_iHeight;
}

void Texture2D::Configure(uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels, unsigned int _uMsaaSamples, uint32_t _uUsage)
{
  m_pAPITexture = api::CreateAPITexture(nullptr, _uWidth, _uHeight, _eFormat, _uMipLevels, _uMsaaSamples, _uUsage, _rSamplerConfig, false);

  m_iWidth = static_cast<int>(_uWidth);
  m_iHeight = static_cast<int>(_uHeight);
}

void Texture2D::ConfigureAsCubemap(const Image* _pImages, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels, unsigned int _uMsaaSamples, uint32_t _uUsage)
{
  if (_pImages != nullptr)
  {
    void* aPDatas[6] = {
      _pImages[0].m_pData,
      _pImages[1].m_pData,
      _pImages[2].m_pData,
      _pImages[3].m_pData,
      _pImages[4].m_pData,
      _pImages[5].m_pData,
    };

    m_pAPITexture = api::CreateAPITexture(aPDatas, _uWidth, _uHeight, _eFormat, _uMipLevels, _uMsaaSamples, _uUsage, _rSamplerConfig, true);
  }
  else
  {
    m_pAPITexture = api::CreateAPITexture(nullptr, _uWidth, _uHeight, _eFormat, _uMipLevels, _uMsaaSamples, _uUsage, _rSamplerConfig, true);
  }

  m_iWidth = static_cast<int>(_uWidth);
  m_iHeight = static_cast<int>(_uHeight);

}

Texture2D::~Texture2D()
{
  api::DestroyAPITexture(m_pAPITexture);
}

void Texture2D::SetupRenderSubState(std::string&& _sName, PipelineStageFlags _uStageFlags, ResourceFrequency _eFrequency) const
{  
  ResourceBindInfo oBindInfo {};
  oBindInfo.m_eLevel = _eFrequency;
  oBindInfo.m_uStageFlags = _uStageFlags;
  oBindInfo.m_sName = _sName;
  api::SubStateSetupTexture(m_pAPITexture, oBindInfo);
}

void Texture2D::SetupAsRenderTargetColor() const
{
  api::RenderTargetAddColorTexture(m_pAPITexture);
}

void Texture2D::SetupAsRenderTargetDepthStencil() const
{
  api::RenderTargetSetDepthStencilTexture(m_pAPITexture);
}

void Texture2D::SetupAsRenderTargetColorResolve() const
{
  api::RenderTargetAddColorResolveTexture(m_pAPITexture);
}

void Texture2D::Bind() const
{
  api::BindAPITexture(m_pAPITexture);
}

void Texture2D::Unbind() const
{
  api::UnbindAPITexture(m_pAPITexture);
}

void Texture2D::ClearAsColor() const
{
  api::ClearAPITexture(m_pAPITexture, TextureUsage::COLOR_TARGET);
}

void Texture2D::ClearAsDepthStencil() const
{
  api::ClearAPITexture(m_pAPITexture, TextureUsage::DEPTH_TARGET);
}
