#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Graphics/PipelineStage.h"

namespace api
{
  namespace dx11
  {  

    struct APIConstantBuffer
    {
      
      Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCBuffer;

      unsigned int m_slot;

      PipelineStageFlags m_uStageFlags;
    };

  }
}