#include "Graphics/Job.h"
#include "Graphics/RenderStateInfo.h"
#include "Graphics/Camera.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Pass.h"
#include "Math/Transform.h"

void Job::UpdateRenderKey(const Camera* _pCamera, const Transform* _pCamTransform, bool _bTranslucent)
{  

  // Distance from camera
  float fDist = glm::distance(_pCamTransform->GetPos(), m_pMeshTransform->GetPos());
  float fNormDist = fmax(fmin((fDist - _pCamera->m_fNear) / _pCamera->m_fFar, 1.f), 0.f);
  if (_bTranslucent)
  {
    fNormDist = 1.f - fNormDist;
  }
  uint64_t uDist = static_cast<uint64_t>(18446744073709551615.0 * static_cast<double>(fNormDist));

  m_xKey = 0u;

  if(_bTranslucent)
  {
    SET_KEY_LAYER_TRANSLUCENT(m_xKey, m_pPass->GetLayer());
    SET_KEY_PASS_TRANSLUCENT(m_xKey, m_pPass->GetId());
    SET_KEY_MATINSTANCE_TRANSLUCENT(m_xKey, m_pMaterial->GetId());
    SET_KEY_DEPTH_TRANSLUCENT(m_xKey, uDist);
    
    //m_xKey |= (static_cast<RenderKey>(uDist) << (KEY_SIZE - 4 - 16 - 64));
  }
  else
  {    
    SET_KEY_LAYER(m_xKey, m_pPass->GetLayer());
    SET_KEY_PASS(m_xKey, m_pPass->GetId());
    SET_KEY_MATINSTANCE(m_xKey, m_pMaterial->GetId());
    SET_KEY_DEPTH(m_xKey, uDist);
  }  
}