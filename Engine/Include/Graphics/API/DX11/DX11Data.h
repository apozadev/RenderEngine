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
      PipelineStage m_eStage;
    };

    std::vector<Resource> m_lstCBuffers;
    std::vector<Resource> m_lstTextures;
  };

  struct DX11Data
  {

    APIWindow* m_pUsingWindow;

    APIRenderTarget* m_pUsingRenderTarget;

    APIRenderState* m_pUsingRenderState;

    RenderTargetBuilder m_oRenderTargetBuilder;

    uint32_t m_uMaxMsaaSamples;

    GlobalLayout m_oGlobalLayout;

  };

}
}