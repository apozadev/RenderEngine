#pragma once

#include <d3d11.h>
#include <wrl.h>

namespace api
{
  namespace dx11
  {  

    struct APIConstantBuffer
    {
      
      Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCBuffer;

      unsigned int m_slot;

      unsigned int m_stageMask = 0u;     
    };

  }
}