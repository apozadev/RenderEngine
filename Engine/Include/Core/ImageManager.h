#pragma once

#include <iostream>
#include <map>
#include <string>

#include "Core/Singleton.h"
#include "Graphics/ImageFormat.h"

struct Image
{
	void* m_pData;
	std::string m_sPath;
	ImageFormat m_eFormat;
	int m_iChannels;
	int m_iWidth, m_iHeight;
	unsigned int GetMemPitch() const { return m_iChannels * m_iWidth; }
};

class ImageManager : public Singleton<ImageManager>
{
public:
	const Image* TryGetLoadedImage(const std::string& _sFilename);
	const Image& LoadImage(const std::string& _sPath, bool _bAbsolute = false);
	const Image& DecodeFromMemory(const std::string& _sPath, unsigned char* _pData, int _iSize);
private:
	std::map<std::string, Image> m_mapLoadedImages;
};