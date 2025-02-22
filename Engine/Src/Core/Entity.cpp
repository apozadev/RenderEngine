#include "Core/Entity.h"

Entity::Entity(Entity&& _rEntity)
{
  m_bEnabled = std::move(_rEntity.m_bEnabled);
  m_bStarted = std::move(_rEntity.m_bStarted);
  m_bTransformDirty = std::move(_rEntity.m_bTransformDirty);
  m_oLocalTransform = std::move(_rEntity.m_oLocalTransform);
  m_oGlobalTransform = std::move(_rEntity.m_oGlobalTransform);  
  m_uNextSiblingId = std::move(_rEntity.m_uNextSiblingId);
  m_uParentId = std::move(_rEntity.m_uParentId);  
  m_lstChildren = std::move(_rEntity.m_lstChildren);
  m_lstComponents = std::move(_rEntity.m_lstComponents);
}

Entity::~Entity()
{
}

Entity& Entity::operator=(Entity&& _rEntity)
{  
  m_lstChildren = std::move(_rEntity.m_lstChildren);
  m_lstComponents = std::move(_rEntity.m_lstComponents);
  return *this;
}

void Entity::Start()
{
  for (owner_ptr<Component>& pComp : m_lstComponents)
  {
    pComp->Start();
  }
}

void Entity::PreTransformUpdate(float _fTimeStep)
{  
  for (owner_ptr<Component>& pComp : m_lstComponents)
  {
    pComp->PreTransformUpdate(_fTimeStep);
  }
}

void Entity::Update(float _fTimeStep)
{ 
  for (owner_ptr<Component>& pComp : m_lstComponents)
  {
    pComp->Update(_fTimeStep);
  }
}