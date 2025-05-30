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
	PipelineStageFlags m_uStageFlags;
	DXGI_FORMAT m_eFormat;
	int m_iWidth, m_iHeight;
	unsigned int m_uLayers;
	unsigned int m_uMipLevels;
};

}
}
