#pragma once

#include "Core/BaseObject.h"

#include "Reflection/ReflectionMacros.h"

#include "Core/ComponentDesc.h"

class Entity;

class Component : public BaseObject
{
  friend class Entity;
  friend class Scene;

public:

  REFLECT_BASE()

  Component() {}
  virtual ~Component() {}

  void SetComponentDesc(owner_ptr<ComponentDesc>&& _pCompDesc) { if (!m_pComponentDesc.get()) { m_pComponentDesc = std::move(_pCompDesc); } }

  const ComponentDesc* GetComponentDesc() { return m_pComponentDesc.get(); }

protected:    

  virtual void Start() {};

  virtual void PreTransformUpdate(float /*_fTimeStep*/) {}
  virtual void Update(float /*_fTimeStep*/) {};

protected:

  owner_ptr<ComponentDesc> m_pComponentDesc;

  Entity* m_pEntity;

  bool m_bStarted = false;
  bool m_bEnabled = false;

};