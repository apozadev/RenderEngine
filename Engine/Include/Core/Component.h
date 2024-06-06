#pragma once

class Entity;

class Component
{
  friend class Entity;
  friend class Scene;

public:

  Component() {}
  virtual ~Component() {}

protected:

  virtual void Start() {};

  virtual void PreTransformUpdate(float /*_fTimeStep*/) {}
  virtual void Update(float /*_fTimeStep*/) {};

protected:

  Entity* m_pEntity;

  bool m_bStarted = false;
  bool m_bEnabled = false;

};