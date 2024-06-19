#pragma once

#include "Core/Exception.h"

#include <vector>
#include <string>

#include <d3d11.h>

namespace api
{
	namespace dx11
	{
		class DX11Exception : public Exception
		{
		public:
			DX11Exception(int _sLine, const char* _sFile, HRESULT _hResult) noexcept;
			const char* GetType() const noexcept override;
		};
	}
}
