#pragma once

#include "Core/TypedPool.h"

#include "Graphics/API/DX11/APIRenderState.h"
#include "Graphics/API/DX11/APIConstantBuffer.h"
#include "Graphics/API/DX11/APIMesh.h"
#include "Graphics/API/DX11/APITexture.h"
#include "Graphics/API/DX11/APIRenderTarget.h"

namespace api
{
namespace dx11
{

  extern TypedPool<APIRenderState, 256>     s_oRenderStatePool;
  extern TypedPool<APIConstantBuffer, 256>  s_oConstantBufferPool;
  extern TypedPool<APIMesh, 256>            s_oMeshPool;
  extern TypedPool<APITexture, 256>         s_oTexturePool;
  extern TypedPool<APIRenderTarget, 256>    s_oRenderTargetPool;

}
}
