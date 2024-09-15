#pragma once

#include <vector>
#include <string>

#include "ImageFormat.h"

struct RenderTargetConfig
{
  std::string m_sId;
  int m_iWidth, m_iHeight;
  unsigned int m_uNumColorTextures;
  ImageFormat m_eFormat;
  unsigned int m_uMipLevels;
  unsigned int m_uMsaaSamples;
  bool m_bHasDepthStencil;
};

struct RenderStepInputConfig
{
  std::string m_sTargetId;
  int m_iImageIdx;
};

struct RenderStepConfig
{
  std::vector<RenderStepInputConfig> m_lstInputs;
  std::string m_sRenderTargetId;
};

struct RenderPipelineConfig
{
  std::vector<RenderStepConfig> m_lstSteps;
  std::vector<RenderTargetConfig> m_lstRenderTargets;
};