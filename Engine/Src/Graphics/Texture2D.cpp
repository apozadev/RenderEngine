
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

  Impl(void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat)
  {    
    m_pAPITexture = api::CreateAPITexture(_pData, _uHeight, _uHeight, _eFormat, 0u);
  }

  ~Impl()
  {
    api::DestroyAPITexture(m_pAPITexture);
  }

};

Texture2D::Texture2D(const std::string& _sFilename, int _iBinding, PipelineStage _eStage)
  : Resource(_iBinding, _eStage)
{
  const Image& rImage = ImageManager::GetInstance()->LoadImage(_sFilename, true);
  m_pImpl = std::make_unique<Impl>(rImage.m_pData, rImage.m_iWidth, rImage.m_iHeight, rImage.m_eFormat);
}

Texture2D::Texture2D(const Image& _rImage, int _iBinding , PipelineStage _eStage)
  : Resource(_iBinding, _eStage)
{  
  m_pImpl = std::make_unique<Impl>(_rImage.m_pData, _rImage.m_iWidth, _rImage.m_iHeight, _rImage.m_eFormat);
}

void Texture2D::Setup(ResourceFrequency _eFrequency) const
{  
  ResourceBindInfo oBindInfo {};
  oBindInfo.m_eLevel = _eFrequency;
  oBindInfo.m_eStage = m_eStage;
  oBindInfo.m_iBinding = m_iBinding;
  api::SubStateSetupTexture(m_pImpl->m_pAPITexture, oBindInfo);
}

void Texture2D::Bind() const
{
  api::BindAPITexture(m_pImpl->m_pAPITexture);
}