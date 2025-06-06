#pragma once

#include <string>

#include "Graphics/BlendEnums.h"
#include "Graphics/CullMode.h"
#include "Graphics/DepthCompareOp.h"
#include "Core/BaseObject.h"
#include "Reflection/ReflectionMacros.h"

struct RenderStateInfo : public BaseObject
{

  REFLECT_BASE()

  uint32_t m_uMeshConstantSize;

  std::string m_sPSFilename;
  std::string m_sGSFilename;
  std::string m_sVSFilename;

  bool m_bBlendEnabled;
  BlendOp m_eBlendOp;
  BlendFactor m_eSrcBlendFactor;
  BlendFactor m_eDstBlendFactor;

  bool m_bDepthWrite;
  bool m_bDepthRead;

  CullMode m_eCullMode;

  DepthCompareOp m_eDepthCompareOp;
};
