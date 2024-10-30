#pragma once

#include <inttypes.h>

#include "Graphics/RenderKey.h"

class Mesh;
class MaterialInstance;
class Pass;
class Window;
class Transform;
class Camera;

struct Job
{
  Mesh* m_pMesh;
  const MaterialInstance* m_pMaterial;
  const Pass* m_pPass;  
  const Transform* m_pMeshTransform;
  RenderKey m_xKey;

  static bool Compare(const Job& j1, const Job& j2)
  {
    return j1.m_xKey > j2.m_xKey;
  }

  void UpdateRenderKey(const Camera* _pCamera, const Transform* _pCamTransform, bool _bTranslucent);
};