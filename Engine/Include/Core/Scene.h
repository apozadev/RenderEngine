#pragma once

#include <memory>
#include <vector>
#include "Core/Entity.h"
#include "Memory/PtrTypes.h"

class Window;

class Scene
{
public:

public:    

  Scene() {}
  Scene(Scene&& _rScene);
  ~Scene();

  Entity* AddEntity(Entity* _pParent = nullptr);  

  void Build();

  void Update(float _fTimeStep);

  Entity* GetParent(Entity* _rEntity);

  Entity* GetChild(Entity* _rEntity, uint32_t _uChildIdx)
  {
    return m_lstEntities[_rEntity->m_lstChildren[_uChildIdx]].get();
  }  

  std::vector<owner_ptr<Entity>>& GetEntities() { return m_lstEntities; }

  Scene& operator=(Scene&& _rScene);

private:    

  uint32_t BuildTraverse(owner_ptr<Entity>& _rEntity, std::vector<owner_ptr<Entity>>& _lstNewScene);

  std::vector<owner_ptr<Entity>> m_lstEntities;    

};