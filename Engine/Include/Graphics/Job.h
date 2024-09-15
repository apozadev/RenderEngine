#pragma once

#include <inttypes.h>

class Mesh;
class MaterialInstance;
class Window;
class Transform;
class Camera;

struct Job
{
  Mesh* m_pMesh;
  const MaterialInstance* m_pMaterial;
  const Window* m_pWindow;
  const Transform* m_pMeshTransform;
  uint64_t m_uKey;

  static bool Compare(const Job& j1, const Job& j2)
  {
    return j1.m_uKey < j2.m_uKey;
  }

  void UpdateRenderKey(Camera* _pCamera, const Transform* _pCamTransform);
};