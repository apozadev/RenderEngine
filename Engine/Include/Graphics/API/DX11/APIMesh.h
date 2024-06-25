#pragma once

#include "Graphics/API/DX11/APIConstantBuffer.h"

namespace api 
{
namespace dx11
{

  struct APIWindow;

  struct APIMesh
  {

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

    unsigned int m_uIndexCount;

    APIConstantBuffer* m_pModelCBuffer;

    APIWindow* m_pOwnerWindow;
  };

}
}
