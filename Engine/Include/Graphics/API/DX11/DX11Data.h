#pragma once

#include <string>
#include <vector>

#include "RenderTargetBuilder.h"
#include "Graphics\PipelineStage.h"

namespace api
{
namespace dx11
{

  struct APIWindow;  
  struct APIRenderTarget;
  struct APIRenderState;

  struct GlobalLayout
  {
    struct Resource
    {
      std::string m_sName;
      uint32_t m_uBinding;
      PipelineStageFlags m_uStageFlags;
    };

    std::vector<Resource> m_lstCBuffers;
    std::vector<Resource> m_lstTextures;
  };

  struct DX11Data
  {
    RenderTargetBuilder m_oRenderTargetBuilder;

    uint32_t m_uMaxMsaaSamples = 1u;

    GlobalLayout m_oGlobalLayout;

  };

}
}