#include "Graphics/RenderStateInfo.h"
#include "Reflection/ReflectionImplMacros.h"

REFLECT_STRUCT_BASE_BEGIN(RenderStateInfo)
REFLECT_STRUCT_MEMBER(m_sVSFilename)
REFLECT_STRUCT_MEMBER(m_sGSFilename)
REFLECT_STRUCT_MEMBER(m_sPSFilename)
REFLECT_STRUCT_MEMBER(m_bBlendEnabled)
REFLECT_STRUCT_MEMBER(m_eBlendOp)
REFLECT_STRUCT_MEMBER(m_eSrcBlendFactor)
REFLECT_STRUCT_MEMBER(m_eDstBlendFactor)
REFLECT_STRUCT_MEMBER(m_bDepthWrite)
REFLECT_STRUCT_MEMBER(m_bDepthRead)
REFLECT_STRUCT_MEMBER(m_eCullMode)
REFLECT_STRUCT_MEMBER(m_eDepthCompareOp)
REFLECT_STRUCT_END(RenderStateInfo)

