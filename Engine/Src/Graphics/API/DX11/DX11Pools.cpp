#include "Graphics/API/DX11/DX11Pools.h"

namespace api
{
namespace dx11
{

  TypedPool<APIRenderState>     s_oRenderStatePool;
  TypedPool<APIConstantBuffer>  s_oConstantBufferPool;
  TypedPool<APIMesh>            s_oMeshPool;
  TypedPool<APITexture>         s_oTexturePool;
  TypedPool<APIRenderTarget>    s_oRenderTargetPool;

}
}