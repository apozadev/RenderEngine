
#include "Graphics/Texture2D.h"
#include "Core/ImageManager.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/Material.h"

class Texture2D::Impl
{
public:
  api::APITexture* m_pAPITexture;

  Impl(void* _pData, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat)
  {    
    m_pAPITexture = api::CreateAPITexture(_pData, _uHeight, _uHeight, _eFormat);
  }

  ~Impl()
  {
    api::DestroyAPITexture(m_pAPITexture);
  }

};

Texture2D::Texture2D(const std::string& _sFilename)
{
  const Image& rImage = ImageManager::GetInstance()->LoadImage(_sFilename);
  m_pImpl = std::make_unique<Impl>(rImage.m_pData, rImage.m_iWidth, rImage.m_iHeight, rImage.m_eFormat);
}

void Texture2D::Setup() const
{
  api::RenderStateSetupTexture(m_pImpl->m_pAPITexture);
}

void Texture2D::Bind() const
{
  api::BindAPITexture(m_pImpl->m_pAPITexture);
}