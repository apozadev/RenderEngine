#pragma once

#include <vector>
#include "Core/Component.h"
#include "Math/Transform.h"

class Entity
{

  friend class Scene;

  struct ConstructKey {};

public:

  ~Entity();

  template<typename T>
  T* AddComponent()
  {
    m_lstComponents.push_back(new T());
    T* pComp = static_cast<T*>(m_lstComponents[m_lstComponents.size() - 1]);
    pComp->m_pEntity = this;
    return pComp;
  }

  uint32_t GetParentId() { return m_uParentId; }

protected:

  void Start();
  void Update(float _fTimeStep);  

public:
  
  const Transform& GetGlobalTransform() { return m_oGlobalTransform; }
  const Transform& GetLocalTransform() { return m_oLocalTransform; }  

  void SetLocalTransform(const Transform& _rTransform)
  {
    m_bTransformDirty = true;
    m_oLocalTransform = _rTransform;
  }

  Entity(ConstructKey&&) {}

private:    

  std::vector<Component*> m_lstComponents;
  std::vector<uint32_t> m_lstChildren;

  Transform m_oGlobalTransform;
  Transform m_oLocalTransform;

  bool m_bTransformDirty = true;

  bool m_bStarted = false;
  bool m_bEnabled = true;  

  uint32_t m_uParentId;
  uint32_t m_uNextSiblingId;
};