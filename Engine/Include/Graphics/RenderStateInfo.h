#pragma once

#include <string>

#include "Graphics/BlendEnums.h"

struct RenderStateInfo
{
  uint32_t m_uMeshConstantSize;

  std::string m_sPSFilename;
  std::string m_sVSFilename;

  bool m_bBlendEnabled;
  BlendOp m_eBlendOp;
  BlendFactor m_eSrcBlendFactor;
  BlendFactor m_eDstBlendFactor;

  bool m_bDepthWrite;
  bool m_bDepthRead;
};
