#pragma once

#include <string>
#include <memory>

#include "Core/BaseObject.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/SamplerConfig.h"

class Material;
struct Image;

class Texture2D : public BaseObject
{
public:
  
  void Configure(const std::string& _sFilename, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  void Configure(const Image& _rImage, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  void Configure(uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u, uint32_t _uUsage = 1u);

  // For cubemaps
  void ConfigureAsCubemap(const Image* _rImages, uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, const SamplerConfig& _rSamplerConfig, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u, uint32_t _uUsage = 1u);

  virtual ~Texture2D();  

  void SetupAsRenderTargetColor() const;
  void SetupAsRenderTargetDepthStencil() const;
  void SetupAsRenderTargetColorResolve() const;

  void GenerateMipMaps() const;

  void Bind() const;
  void Unbind() const;

  void ClearAsColor() const;
  void ClearAsDepthStencil() const;

  int GetWidth() const { return m_iWidth; };
  int GetHeight() const { return m_iHeight; };

public:

  api::APITexture* m_pAPITexture;

private:  

  PipelineStage m_eStage;

  int m_iWidth = 0, m_iHeight = 0;

};