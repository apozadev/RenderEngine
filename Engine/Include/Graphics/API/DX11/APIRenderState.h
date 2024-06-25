#pragma once

#include <d3d11.h>
#include <wrl.h>

namespace api
{
namespace dx11
{

  struct APIWindow;

  struct APIRenderState
  {

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;    

    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizer;

    APIWindow* m_pOwnerWindow;

  };

}
}