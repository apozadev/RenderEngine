#pragma once

enum class TextureAddressMode
{
  REPEAT,
  MIRRORED_REPEAT,
  CLAMP,
  BORDER
};

enum class TextureFilterMode
{
  POINT,
  LINEAR
};

struct SamplerConfig
{
  TextureAddressMode m_eAddressMode;
  TextureFilterMode m_eMipmapFilterMode;
  TextureFilterMode m_eMinFilterMode;
  TextureFilterMode m_eMagFilterMode;
};
