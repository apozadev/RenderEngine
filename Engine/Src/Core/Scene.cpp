#include "Core/Scene.h"
#include "Core/Exception.h"
#include "Core/Engine.h"
#include "Math/Utils.h"
#include "Memory/Factory.h"

#include <stack>
#include <cstdint>

Scene::Scene(Scene&& _rScene)
  : m_lstEntities(std::move(_rScene.m_lstEntities))
{

}

Scene& Scene::operator=(Scene&& _rScene)
{
  m_lstEntities = std::move(_rScene.m_lstEntities);
  return *this;
}

Scene::~Scene()
{

}

Entity* Scene::AddEntity(Entity* _pParent)
{
  uint32_t uParentId = _MAX_UINT32;

  if (_pParent != nullptr)
  {
    std::uintptr_t uBeginPtr = reinterpret_cast<std::uintptr_t>(m_lstEntities.data());
    uParentId = static_cast<uint32_t>((reinterpret_cast<std::uintptr_t>(_pParent) - uBeginPtr) / sizeof(Entity*));
  }

  if (uParentId  == _MAX_UINT32 || uParentId < m_lstEntities.size())
  {    
    m_lstEntities.push_back(Factory::Create<Entity>());

    Entity* pEntity = m_lstEntities.back().get();

    pEntity->m_uParentId = uParentId;

    if (uParentId != _MAX_UINT32)
    {      
      Entity* pParent = m_lstEntities[uParentId].get();
      pEntity->m_lstChildren.push_back(static_cast<uint32_t>(m_lstEntities.size() - 1));
    }    

    if (Engine::GetInstance()->IsRunning())
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
  std::vector<owner_ptr<Entity>> lstNewScene;
  lstNewScene.reserve(m_lstEntities.size());
  
  // Traverse top-level entities (depth first) and fill new vector
  for (auto& rEntityPtr : m_lstEntities)
  {
    if (GetParent(rEntityPtr.get()) == nullptr)
    {
      BuildTraverse(rEntityPtr, lstNewScene);
    }
  }

  if (lstNewScene.size() != m_lstEntities.size())
  {
    THROW_GENERIC_EXCEPTION("Scene building failed");
  }

  // keep new list
  m_lstEntities.swap(lstNewScene);
}

uint32_t Scene::BuildTraverse(owner_ptr<Entity>& _pEntity, std::vector<owner_ptr<Entity>>& _lstNewScene)
{
  uint32_t uCurrId = _lstNewScene.size();

  _lstNewScene.push_back(std::move(_pEntity ));
  owner_ptr<Entity>& rNewEntity = _lstNewScene[uCurrId];

  for (owner_ptr<Component>& pComponent : rNewEntity->m_lstComponents)
  {
    pComponent->m_pEntity = rNewEntity.get();
  }

  for (uint32_t& uChildId : rNewEntity->m_lstChildren)
  {
    auto& rChild = m_lstEntities[uChildId];
    rChild->m_uParentId = uCurrId;
    uChildId = BuildTraverse(rChild, _lstNewScene);
  }

  rNewEntity->m_uNextSiblingId = _lstNewScene.size();

  return uCurrId;
}

Entity* Scene::GetParent(Entity* _pEntity)
{
  if (_pEntity->m_uParentId == _MAX_UINT32)
  {
    return nullptr;
  }

  return m_lstEntities[_pEntity->m_uParentId].get();
}

void Scene::Update(float _fTimeStep)
{  

  unsigned int uNumEntities = static_cast<unsigned int>(m_lstEntities.size());

  std::stack<Transform> lstTransformStack;

  // Update transforms
  for (unsigned int uIdx = 0u; uIdx < uNumEntities; uIdx++)
  {
    auto& rEntityPtr = m_lstEntities[uIdx];

    if (rEntityPtr->m_bEnabled)
    {

      rEntityPtr->PreTransformUpdate(_fTimeStep);

      if (rEntityPtr->m_bTransformDirty)
      {
        if (!lstTransformStack.empty())
        {
          rEntityPtr->m_oGlobalTransform = lstTransformStack.top() * rEntityPtr->GetLocalTransform();
        }
        else
        {
          rEntityPtr->m_oGlobalTransform = rEntityPtr->GetLocalTransform();
        }
        rEntityPtr->m_bTransformDirty = false;
      }

      if (uIdx < uNumEntities - 1)
      {
        uint32_t uNextParentIdx = m_lstEntities[uIdx + 1]->GetParentId();
        if (uNextParentIdx == uIdx)
        {
          lstTransformStack.push(rEntityPtr->m_oGlobalTransform);
        }
        else if (uNextParentIdx != rEntityPtr->GetParentId())
        {
          lstTransformStack.pop();
        }
      }
    }
    else
    {
      uIdx = rEntityPtr->m_uNextSiblingId - 1u;
    }
  }

  // Update logic
  for (unsigned int uIdx = 0u; uIdx < uNumEntities; uIdx++)
  {
    auto& rEntityPtr = m_lstEntities[uIdx];
    if (rEntityPtr->m_bEnabled)
    {
      if (!rEntityPtr->m_bStarted)
      {
        rEntityPtr->Start();
        rEntityPtr->m_bStarted = true;
      }
      rEntityPtr->Update(_fTimeStep);
    }
    else
    {
      uIdx = rEntityPtr->m_uNextSiblingId - 1u;
    }
  }
}
