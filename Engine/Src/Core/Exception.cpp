#include "Core/Exception.h"
#include <sstream>

namespace core {

Exception::Exception(int _iLine, const char* _sFile, const char* _sDesc) noexcept
	:
	m_iLine(_iLine), m_sFile(_sFile), m_sDescription(_sDesc)
{	
}

const char* Exception::what() const noexcept
{	
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString() << std::endl
		<< m_sDescription << std::endl;
	m_sDescription = oss.str();
	return m_sDescription.c_str();
}

int Exception::GetLine() const noexcept
{
	return m_iLine;
}

const std::string& Exception::GetFile() const noexcept
{
	return m_sFile;
}

const char* Exception::GetType() const noexcept { return "Generic Exception"; }

std::string Exception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << m_sFile << std::endl
		<< "[Line]" << m_iLine;
	return oss.str();
}

}
