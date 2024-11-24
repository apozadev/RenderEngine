#pragma once

#include <string>
#include <memory>

#include "Core/PooledObject.h"
#include "Graphics/ImageFormat.h"
#include "Graphics/API/GraphicsAPI.h"

class Material;
struct Image;

class Texture2D : public TypedPooledObject<Texture2D, 256>
{
public:

  using TypedPooledObject<Texture2D, 256>::TypedPooledObject;
  
  void Configure(const std::string& _sFilename, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  void Configure(const Image& _rImage, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u);
  void Configure(uint32_t _uWidth, uint32_t _uHeight, ImageFormat _eFormat, int _iBinding, PipelineStage _eStage, unsigned int _uMipLevels = 0u, unsigned int _uMsaaSamples = 1u, uint32_t _uUsage = 1u);  

  virtual ~Texture2D();

  void SetupRenderSubState(ResourceFrequency _eFrequency) const;

  void SetupAsRenderTargetColor() const;
  void SetupAsRenderTargetDepthStencil() const;
  void SetupAsRenderTargetColorResolve() const;

  void Bind() const;

  void ClearAsColor() const;
  void ClearAsDepthStencil() const;

private:

  int m_iBinding;

  PipelineStage m_eStage;

  api::APITexture* m_pAPITexture;

};