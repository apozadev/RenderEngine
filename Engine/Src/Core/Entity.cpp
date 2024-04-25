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
  for (Component* pComp : m_lstComponents)
  {
    pComp->Update(_fTimeStep);
  }
}