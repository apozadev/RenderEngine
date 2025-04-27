#pragma once

#include <memory>
#include <vector>

#include "ImageFormat.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Texture2D.h"
#include "Memory/PtrTypes.h"

class RenderTarget
{
public:
  
  ~RenderTarget();

  void Configure(unsigned int _uNumColorAttachments, unsigned int _uWidth, unsigned int _uHeight, ImageFormat _eFormat, bool _bHasDepthStencil, unsigned int _uMipLevels = 1u, unsigned int _uMsaaSamples = 1u, bool _bIsCubemap = false);

  void SetUsing() const;

  void Bind() const;

  void Unbind() const;

  bool IsBound() const;

  void Clear() const;

  const std::vector<owner_ptr<Texture2D>>& RenderTarget::GetColorTextures() const;

  Texture2D* GetDepthStencilTexture() const;

  uint32_t GetMsaaSamples() const;

  void Resize();

private:

  api::APIRenderTarget* m_pAPIRenderTarget;

  std::vector<owner_ptr<Texture2D>> m_lstColorTextures;

  std::vector<owner_ptr<Texture2D>> m_lstColorResolveTextures;

  owner_ptr<Texture2D> m_pDepthStencilTexture;

  uint32_t m_uMsaaSamples;
  
};
