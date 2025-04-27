#pragma once

#include <d3d11.h>
#include <D3dCompiler.h>
#include <wrl.h>

namespace api
{
namespace dx11
{

  struct APIWindow;

  struct APIRenderState
  {

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;    

    Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_pGeometryShader;
    
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizer;

    Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDSState;

    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_pVertexReflection;

    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_pGeometryReflection;

    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_pPixelReflection;

  };

}
}