#pragma once

class Entity;

class Component
{
  friend class Entity;

protected:

  virtual void Start() {};
  virtual void Update(float /*_fTimeStep*/) {};

protected:

  Entity* m_pEntity;

  bool m_bStarted = false;
  bool m_bEnabled = false;

};