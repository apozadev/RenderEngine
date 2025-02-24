#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "Core/ImageManager.h"
#include "Core/Exception.h"
#include "File/FileUtils.h"

template<typename T>
void RGB2RGBA(T** _ppData, int _iWidth, int _iHeight)
{
  T* oldData = *_ppData;
  T* newData = (T*)malloc(_iWidth * _iHeight * sizeof(T) * 4);
  int old_i = 0;
  int new_i = 0;
  for (int i = 0; i < _iWidth * _iHeight; i++)
  {
    newData[new_i++] = oldData[old_i++];
    newData[new_i++] = oldData[old_i++];
    newData[new_i++] = oldData[old_i++];
    newData[new_i++] = 1;
  }
  free(oldData);
  *_ppData = newData;
}

const Image* ImageManager::TryGetLoadedImage(const std::string& _sFilename)
{  
  if (!_sFilename.empty() && m_mapLoadedImages.find(_sFilename) != m_mapLoadedImages.end())
  {
    return &m_mapLoadedImages[_sFilename];
  }
  return nullptr;
}

const Image& ImageManager::DecodeFromMemory(const std::string& _sPath, unsigned char* _pData, int _iSize)
{

  const Image* loadedImg = TryGetLoadedImage(_sPath);
  if (loadedImg)
  {
    return *loadedImg;
  }

  Image img;
  img.m_sPath = _sPath;
  try
  {
    img.m_pData = stbi_load_from_memory(_pData, _iSize, &(img.m_iWidth), &(img.m_iHeight), &(img.m_iChannels), 0);
  }
  catch (const char* err)
  {
    std::cout << err << std::endl;
  }
  switch (img.m_iChannels)
  {
  /*case 1:
    img.format = R8;
    break;
  case 2:
    img.format = R8G8;
    break;*/
  case 3:
  {
    RGB2RGBA((unsigned char**)(&img.m_pData), img.m_iWidth, img.m_iHeight);
    img.m_eFormat = ImageFormat::R8G8B8A8;
    img.m_iChannels = 4;
  }
  break;
  case 4:
    img.m_eFormat = ImageFormat::R8G8B8A8;
    break;
  default:
    THROW_GENERIC_EXCEPTION("Trying to load image with unsuported format");
    break;
  }

  if (!_sPath.empty())
  {
    return m_mapLoadedImages.emplace(_sPath, img).first->second;
  }
  else
  {
    return m_mapLoadedImages.emplace(std::string("Texture_") + std::to_string(m_mapLoadedImages.size()), img).first->second;
  }
}

const Image& ImageManager::LoadImage(const std::string& _sPath, bool _bAbsolute/* = false*/)
{

  const Image* loadedImg = TryGetLoadedImage(_sPath);
  if (loadedImg)
  {
    return *loadedImg;
  }

  Image img;
  img.m_sPath = _sPath;
  bool isHDR = false;
  try
  {
    size_t extIdx = _sPath.find_last_of('.');
    if (extIdx != std::string::npos && extIdx < _sPath.size() - 1)
    {
      std::string extension = _sPath.substr(extIdx + 1);
      isHDR = extension == "exr" || extension == "hdr";
    }

    std::string sFullPath = _bAbsolute? _sPath : file::GetWorkingDirectory() + _sPath;

    if (isHDR)
    {
      img.m_pData = stbi_loadf(sFullPath.c_str(), &(img.m_iWidth), &(img.m_iHeight), &(img.m_iChannels), 0);
    }
    else
    {
      img.m_pData = stbi_load(sFullPath.c_str(), &(img.m_iWidth), &(img.m_iHeight), &(img.m_iChannels), 0);
    }
  }
  catch (const char* err)
  {
    std::cout << err << std::endl;
  }
  if (isHDR)
  {
    switch (img.m_iChannels)
    {
    /*case 1:
      img.format = DXGI_FORMAT_R32_FLOAT;
      break;
    case 2:
      img.format = DXGI_FORMAT_R32G32_FLOAT;
      break;*/
    case 3:
      //RGB2RGBA((float**)(&img.data), img.width, img.height);
      //img.format = DXGI_FORMAT_R32G32B32A32_FLOAT;

      //img.format = DXGI_FORMAT_R32G32B32_FLOAT;
      break;
    case 4:
      //img.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
      break;
    }
  }
  else
  {
    switch (img.m_iChannels)
    {
   /* case 1:
      img.format = DXGI_FORMAT_R8_UNORM;
      break;
    case 2:
      img.format = DXGI_FORMAT_R8G8_UNORM;
      break;*/
    case 3:
      RGB2RGBA((unsigned char**)(&img.m_pData), img.m_iWidth, img.m_iHeight);
      img.m_eFormat = ImageFormat::R8G8B8A8;
      img.m_iChannels = 4;
      break;
    case 4:
      img.m_eFormat = ImageFormat::R8G8B8A8;
      break;
    }
  }

  return m_mapLoadedImages.emplace(_sPath, img).first->second;
}