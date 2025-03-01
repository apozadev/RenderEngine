#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Graphics/PipelineStage.h"

namespace api
{
namespace dx11
{

struct APIWindow;

struct APITexture
{

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	unsigned int m_uSlot;
	PipelineStage m_eStage;
	DXGI_FORMAT m_eFormat;
	int m_iWidth, m_iHeight;

};

}
}
