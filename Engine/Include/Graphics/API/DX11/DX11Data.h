#pragma once

#include "RenderTargetBuilder.h"

namespace api
{
namespace dx11
{

  struct APIWindow;  
  struct APIRenderTarget;

  struct DX11Data
  {

    APIWindow* m_pUsingWindow;

    APIRenderTarget* m_pUsingRenderTarget;

    RenderTargetBuilder m_oRenderTargetBuilder;

  };

}
}