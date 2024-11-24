#pragma once

#include <memory>
#include <vector>

#include "ImageFormat.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Texture2D.h"

class RenderTarget
{
public:

  RenderTarget(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels = 1u, unsigned int _uMsaaSamples = 1u);
  ~RenderTarget();

  void SetUsing() const;

  void Bind() const;

  void Unbind() const;

  void Clear() const;

  const std::vector<pooled_ptr<Texture2D>>& RenderTarget::GetColorTextures();

  Texture2D* GetDepthStencilTexture();

  uint32_t GetMsaaSamples() const;

  void Resize();

private:

  api::APIRenderTarget* m_pAPIRenderTarget;

  std::vector<pooled_ptr<Texture2D>> m_lstColorTextures;

  std::vector<pooled_ptr<Texture2D>> m_lstColorResolveTextures;

  pooled_ptr<Texture2D> m_pDepthStencilTexture;

  uint32_t m_uMsaaSamples;
  
};
