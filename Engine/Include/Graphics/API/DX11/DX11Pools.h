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

  extern TypedPool<APIRenderState>     s_oRenderStatePool;
  extern TypedPool<APIConstantBuffer>  s_oConstantBufferPool;
  extern TypedPool<APIMesh>            s_oMeshPool;
  extern TypedPool<APITexture>         s_oTexturePool;
  extern TypedPool<APIRenderTarget>    s_oRenderTargetPool;

}
}
