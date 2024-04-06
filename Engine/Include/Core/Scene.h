#pragma once

#include <vector>
#include "Core/Entity.h"

class Scene
{
public:

public:    

  Entity* AddEntity(Entity* _pParent = nullptr);  

  void Build();

  void Update(float _fTimeStep);

  Entity* GetParent(Entity& _rEntity);

  Entity* GetChild(Entity& _rEntity, uint32_t _uChildIdx)
  {
    return &m_lstEntities[_rEntity.m_lstChildren[_uChildIdx]];
  }  

private:    

  uint32_t BuildTraverse(Entity& _rEntity, std::vector<Entity>& _lstNewScene);

  std::vector<Entity> m_lstEntities;    
};