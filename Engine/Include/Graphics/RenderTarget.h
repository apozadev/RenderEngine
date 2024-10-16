#pragma once

#include <memory>
#include <vector>

#include "ImageFormat.h"

class Texture2D;

class RenderTarget
{
public:

  RenderTarget(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels = 1u, unsigned int _uMsaaSamples = 1u);
  ~RenderTarget();

  void SetUsing() const;

  void Bind() const;

  void Unbind() const;

  std::vector<Texture2D*> RenderTarget::GetColorTextures();

  Texture2D* GetDepthStencilTexture();

  uint32_t GetMsaaSamples() const;

  void Resize();

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;
  
};
