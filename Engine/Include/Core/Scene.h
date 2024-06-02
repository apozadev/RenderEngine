#pragma once

#include <vector>
#include "Core/Entity.h"

class Window;

class Scene
{
public:

public:    

  Scene(Window* _pWindow) : m_pWindow(_pWindow) {}
  ~Scene();

  Entity* AddEntity(Entity* _pParent = nullptr);  

  void Build();

  void Update(float _fTimeStep);

  Entity* GetParent(Entity& _rEntity);

  Entity* GetChild(Entity& _rEntity, uint32_t _uChildIdx)
  {
    return &m_lstEntities[_rEntity.m_lstChildren[_uChildIdx]];
  }  

  Window* GetWindow() const { return m_pWindow; }

private:    

  uint32_t BuildTraverse(Entity& _rEntity, std::vector<Entity>& _lstNewScene);

  std::vector<Entity> m_lstEntities;    

  Window* m_pWindow;


};