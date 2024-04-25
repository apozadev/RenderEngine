#pragma once

#include <exception>
#include <string>

#define THROW_GENERIC_EXCEPTION(description) throw Exception(__LINE__,__FILE__,description);

class Exception : public std::exception
{
public:
	Exception(int _iLine, const char* _sFile, const char* _sDesc) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int m_iLine;
	std::string m_sFile;
protected:
	mutable std::string m_sDescription;
};

