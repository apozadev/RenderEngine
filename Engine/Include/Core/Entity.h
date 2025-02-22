#pragma once

#include <memory>
#include <vector>
#include "Memory/PtrTypes.h"
#include "Memory/Factory.h"
#include "Core/Component.h"
#include "Math/Transform.h"

class Entity
{

  friend class Scene;  

public:  

  Entity() = default;  
  Entity(Entity&& _rEntity);
  ~Entity();

  template<class T, typename ...Args>
  T* AddComponent(Args&&... args)
  {
    m_lstComponents.push_back(owner_ptr<Component>((Component*)(Factory::Create<T>(std::forward<Args>(args)...).release())));
    T* pComp = static_cast<T*>(m_lstComponents[m_lstComponents.size() - 1].get());
    pComp->m_pEntity = this;
    return pComp;
  }

  void AddComponent(owner_ptr<Component>&& _pComponent)
  {
    _pComponent->m_pEntity = this;
    m_lstComponents.push_back(std::move(_pComponent));
  }

  const std::vector<owner_ptr<Component>>& GetComponents() const { return m_lstComponents;}

  uint32_t GetParentId() { return m_uParentId; }

  Entity& operator=(Entity&& _rEntity);

protected:

  void Start();
  void PreTransformUpdate(float _fTimeStep);
  void Update(float _fTimeStep);  

public:
  
  const Transform& GetGlobalTransform() { return m_oGlobalTransform; }
  const Transform& GetLocalTransform() { return m_oLocalTransform; }  
  Transform& GetMutableLocalTransform() { m_bTransformDirty = true;  return m_oLocalTransform; }

  void SetLocalTransform(const Transform& _rTransform)
  {
    m_bTransformDirty = true;
    m_oLocalTransform = _rTransform;
  }    

private:    

  std::vector<owner_ptr<Component>> m_lstComponents;
  std::vector<uint32_t> m_lstChildren;

  Transform m_oGlobalTransform;
  Transform m_oLocalTransform;

  bool m_bTransformDirty = true;

  bool m_bStarted = false;
  bool m_bEnabled = true;  

  uint32_t m_uParentId;
  uint32_t m_uNextSiblingId;
};