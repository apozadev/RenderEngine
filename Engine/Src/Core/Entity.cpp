#include "Core/Entity.h"

void Entity::Start()
{
  for (Component* pComp : m_vctComponents)
  {
    pComp->Start();
  }
}

void Entity::Update(float _fTimeStep)
{
  for (Component* pComp : m_vctComponents)
  {
    pComp->Update(_fTimeStep);
  }
}