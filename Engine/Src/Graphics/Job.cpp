#include "Graphics/Job.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/Camera.h"
#include"Graphics/MaterialInstance.h"
#include"Graphics/Material.h"
#include "Math/Transform.h"

#define KEYPOS_LAST 64

#define GEN_KEY(value, size, pos) static_cast<uint64_t>(value) << (KEYPOS_LAST - size - pos)

void Job::UpdateRenderKey(Camera* _pCamera, const Transform* _pCamTransform)
{
  const RenderStateInfo& rRSInfo = m_pMaterial->GetMaterial()->GetRenderStateInfo();
  bool bTranslucent = rRSInfo.m_bBlendEnabled;

  m_uKey = 0u;

  // Window id
  m_uKey |= GEN_KEY(m_pWindow->GetId(), 8, KEYPOS_LAST);

  // Translucent? 
  m_uKey |= GEN_KEY(bTranslucent, 1, KEYPOS_LAST - 8);

  // Distance from camera

  float fDist = glm::distance(_pCamTransform->GetPos(), m_pMeshTransform->GetPos());
  float fNormDist = fmax(fmin((fDist - _pCamera->m_fNear) / _pCamera->m_fFar, 1.f), 0.f);

  if (bTranslucent)
  {
    fNormDist = 1.f - fNormDist;
  }

  uint32_t uDist = static_cast<uint32_t>(4294967295.0f * fNormDist);

  m_uKey |= GEN_KEY(uDist, 32, KEYPOS_LAST - 9);
}