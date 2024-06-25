#pragma once

#include <string>

std::wstring CharToLPWSTR(const char* cs)
{
	size_t size = 1000;
	size_t i;
	size_t length = strlen(cs) + 1;
	wchar_t* buffer = (wchar_t*)malloc(length * sizeof(wchar_t));
	mbsrtowcs_s(&i, buffer, length, &cs, length - 1, NULL);
	std::wstring wstr(buffer);
	free(buffer);
	return wstr;
}