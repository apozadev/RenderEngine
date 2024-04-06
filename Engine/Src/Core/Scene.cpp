#include "Core/Scene.h"
#include "Core/Exception.h"
#include "Core/Engine.h"
#include "Math/Utils.h"

#include <stack>
#include <cstdint>

Entity* Scene::AddEntity(Entity* _pParent)
{
  uint32_t uParentId = _MAX_UINT32;

  if (_pParent != nullptr)
  {
    std::uintptr_t uBeginPtr = reinterpret_cast<std::uintptr_t>(m_lstEntities.data());
    uParentId = static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(_pParent) - uBeginPtr);
  }

  if (uParentId  == _MAX_UINT32 || uParentId < m_lstEntities.size())
  {
    m_lstEntities.push_back(Entity(Entity::ConstructKey()));
    Entity* pEntity = &m_lstEntities[m_lstEntities.size()-1];
    pEntity->m_uParentId = uParentId;
    pEntity->m_lstChildren.push_back(static_cast<uint32_t>(m_lstEntities.size() - 1));

    if (core::Engine::GetInstance()->IsRunning())
    {
      Build();
    }

    return pEntity;
  }

  THROW_GENERIC_EXCEPTION("Invalid parent entity pointer")

  return nullptr;
}

void Scene::Build()
{
  // create new vector for ordered entities
  std::vector<Entity> lstNewScene;
  lstNewScene.reserve(m_lstEntities.size());
  
  // Traverse top-level entities (depth first) and fill new vector
  for (Entity& rEntity : m_lstEntities)
  {
    if (GetParent(rEntity) == nullptr)
    {
      BuildTraverse(rEntity, lstNewScene);
    }
  }

  if (lstNewScene.size() != m_lstEntities.size())
  {
    THROW_GENERIC_EXCEPTION("Scene building failed");
  }

  // keep new list
  m_lstEntities.swap(lstNewScene);
}

uint32_t Scene::BuildTraverse(Entity& _rEntity, std::vector<Entity>& _lstNewScene)
{
  uint32_t uCurrId = _lstNewScene.size();

  _lstNewScene.push_back(std::move(_rEntity));
  Entity& rNewEntity = _lstNewScene[uCurrId];

  for (uint32_t& uChildId : rNewEntity.m_lstChildren)
  {
    Entity& rChild = m_lstEntities[uChildId];
    rChild.m_uParentId = uCurrId;
    uChildId = BuildTraverse(rChild, _lstNewScene);
  }

  rNewEntity.m_uNextSiblingId = _lstNewScene.size();

  return uCurrId;
}

Entity* Scene::GetParent(Entity& _rEntity)
{
  if (_rEntity.m_uParentId == _MAX_UINT32)
  {
    return nullptr;
  }

  return &m_lstEntities[_rEntity.m_uParentId];
}

void Scene::Update(float _fTimeStep)
{  

  unsigned int uNumEntities = static_cast<unsigned int>(m_lstEntities.size());

  std::stack<Transform> lstTransformStack;

  // Update transforms
  for (unsigned int uIdx = 0u; uIdx < uNumEntities; uIdx++)
  {
    Entity& rEntity = m_lstEntities[uIdx];

    if (rEntity.m_bEnabled)
    {
      if (rEntity.m_bTransformDirty)
      {
        if (!lstTransformStack.empty())
        {
          rEntity.m_oGlobalTransform = lstTransformStack.top() * rEntity.GetLocalTransform();
        }
        else
        {
          rEntity.m_oGlobalTransform = rEntity.GetLocalTransform();
        }
        rEntity.m_bTransformDirty = false;
      }

      if (uIdx < uNumEntities - 1)
      {
        uint32_t uNextParentIdx = m_lstEntities[uIdx + 1].GetParentId();
        if (uNextParentIdx == uIdx)
        {
          lstTransformStack.push(rEntity.m_oGlobalTransform);
        }
        else if (uNextParentIdx != rEntity.GetParentId())
        {
          lstTransformStack.pop();
        }
      }
    }
    else
    {
      uIdx = rEntity.m_uNextSiblingId - 1u;
    }
  }

  // Update logic
  for (unsigned int uIdx = 0u; uIdx < uNumEntities; uIdx++)
  {
    Entity& rEntity = m_lstEntities[uIdx];
    if (rEntity.m_bEnabled)
    {
      if (!rEntity.m_bStarted)
      {
        rEntity.Start();
        rEntity.m_bStarted = true;
      }
      rEntity.Update(_fTimeStep);
    }
    else
    {
      uIdx = rEntity.m_uNextSiblingId - 1u;
    }
  }
}
