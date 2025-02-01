#pragma once

#include <string>
#include <memory>

#include "Core/BaseObject.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/API/GraphicsAPI.h"

class Material;
struct Image;

class Texture2D : public BaseObject
{
public:
  
  void Configure(const std::string& _sFilename, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  void Configure(const Image& _rImage, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  void Configure(uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u, uint32_t _uUsage = 1u);

  virtual ~Texture2D();

  void SetupRenderSubState(std::string&& _sName, PipelineStage _eStage, ResourceFrequency _eFrequency) const;

  void SetupAsRenderTargetColor() const;
  void SetupAsRenderTargetDepthStencil() const;
  void SetupAsRenderTargetColorResolve() const;

  void Bind() const;
  void Unbind() const;

  void ClearAsColor() const;
  void ClearAsDepthStencil() const;

  int GetWidth() const { return m_iWidth; };
  int GetHeight() const { return m_iHeight; };

private:  

  PipelineStage m_eStage;

  api::APITexture* m_pAPITexture;

  int m_iWidth = 0, m_iHeight = 0;

};