#include "Core/Entity.h"

Entity::~Entity()
{
  for (Component* pComponent : m_lstComponents)
  {
    delete pComponent;
  }
}

void Entity::Start()
{
  for (Component* pComp : m_lstComponents)
  {
    pComp->Start();
  }
}

void Entity::Update(float _fTimeStep)
{
  constexpr float fSpeed = 1.f;
  m_oLocalTransform.SetRot(m_oLocalTransform.GetRot() * glm::quat(glm::vec3(0.f, fSpeed * _fTimeStep, 0.f)));

  m_bTransformDirty = true;

  for (Component* pComp : m_lstComponents)
  {
    pComp->Update(_fTimeStep);
  }
}