#pragma once

#include <memory>
#include <vector>
#include "Core/Entity.h"

class Window;

class Scene
{
public:

public:    

  Scene(Window* _pWindow) : m_pWindow(_pWindow) {}
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

  Window* GetWindow() const { return m_pWindow; }

  Scene& operator=(Scene&& _rScene);

private:    

  uint32_t BuildTraverse(std::unique_ptr<Entity>& _rEntity, std::vector<std::unique_ptr<Entity>>& _lstNewScene);

  std::vector<std::unique_ptr<Entity>> m_lstEntities;    

  Window* m_pWindow;


};