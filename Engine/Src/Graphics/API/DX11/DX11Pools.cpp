#include "Graphics/API/DX11/DX11Pools.h"

namespace api
{
namespace dx11
{

  TypedPool<APIRenderState, 256>     s_oRenderStatePool;
  TypedPool<APIConstantBuffer, 256>  s_oConstantBufferPool;
  TypedPool<APIMesh, 256>            s_oMeshPool;
  TypedPool<APITexture, 256>         s_oTexturePool;
  TypedPool<APIRenderTarget, 256>    s_oRenderTargetPool;

}
}