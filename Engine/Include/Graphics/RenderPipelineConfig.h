#pragma once

#include <vector>
#include <string>

#include "ImageFormat.h"
#include "Core/BaseObject.h"

#include "Reflection/ReflectionMacros.h"

struct RenderTargetConfig : public BaseObject
{
  REFLECT_BASE()

  std::string m_sId;
  int m_iWidth, m_iHeight;
  unsigned int m_uNumColorTextures;
  ImageFormat m_eFormat;
  unsigned int m_uMipLevels;
  unsigned int m_uMsaaSamples;
  bool m_bHasDepthStencil;
};

struct RenderStepInputConfig : public BaseObject
{
  REFLECT_BASE()

  std::string m_sTargetId;
  int m_iImageIdx;
};

struct RenderStepConfig : public BaseObject
{
  REFLECT_BASE()

  std::vector<RenderStepInputConfig> m_lstInputs;
  std::string m_sRenderTargetId;
  bool m_bOrderTranslucent;
};

struct RenderPipelineConfig : public BaseObject
{
  REFLECT_BASE()

  std::string m_sId;
  std::vector<RenderStepConfig> m_lstSteps;
  std::vector<RenderTargetConfig> m_lstRenderTargets;
};

DECLARE_REFLECTION_VECTOR(RenderStepInputConfig)
DECLARE_REFLECTION_VECTOR(RenderStepConfig)
DECLARE_REFLECTION_VECTOR(RenderTargetConfig)
