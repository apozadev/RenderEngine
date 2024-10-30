#pragma once

#include <string>
#include <memory>

#include "Graphics/Resource.h"
#include "Graphics/ImageFormat.h"

class Material;
struct Image;

class Texture2D : public Resource
{
public:
  //Texture2D() {};
  Texture2D(const std::string& _sFilename, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  Texture2D(const Image& _rImage, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  Texture2D(uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u, uint32_t _uUsage = 1u);

  virtual ~Texture2D() = default;

  void SetupRenderSubState(ResourceFrequency _eFrequency) const override;

  void SetupAsRenderTargetColor() const;
  void SetupAsRenderTargetDepthStencil() const;
  void SetupAsRenderTargetColorResolve() const;

  void Bind() const override;

  void ClearAsColor() const;
  void ClearAsDepthStencil() const;

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;

};