#include "Graphics\API/DX11/DX11Exception.h"

#include <sstream>

#include "../3rd/dxerr/dxerr.h"

namespace api
{
	namespace dx11
	{

		DX11Exception::DX11Exception(int _sLine, const char* _sFile, HRESULT _hResult) noexcept
			: Exception(_sLine, _sFile, "")
		{
			std::ostringstream oss;

			char dDescription[512];
			DXGetErrorDescription(_hResult, dDescription, sizeof(dDescription));

			oss << GetType() << std::endl
				<< "[Error Code] 0x" << std::hex << std::uppercase << _hResult
				<< std::dec << " (" << (unsigned long)_hResult << ")" << std::endl
				<< "[Error String] " << DXGetErrorString(_hResult) << std::endl
				<< "[Description] " << dDescription << std::endl;
			oss << GetOriginString();
			m_sDescription = oss.str();			
		}

		const char* DX11Exception::GetType() const noexcept
		{
			return "DX11 Exception";
		}

	}
}
