#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_ext.hpp"

#include "Core/SceneLoading.h"
#include "Core/Scene.h"
#include "Core/SceneDesc.h"
#include "Core/ComponentDesc.h"

#include "Memory/PtrTypes.h"

#include "Reflection/DeserializationTypeVisitor.h"
#include "Reflection/CopyTypeVisitor.h"
#include "Reflection/ReflectionHelper.h"

void LoadScene(const char* _sFilename, Scene& rScene_)
{
  SceneDesc oSceneDesc;

  reflection::ReflectionHelper::Deserialize(_sFilename, &oSceneDesc);

  for (const owner_ptr<EntityDesc>& pEntityDesc : oSceneDesc.Entities)
  {
    Entity* pEntity = rScene_.AddEntity();

    Transform& oTr = pEntity->GetMutableLocalTransform();
    oTr.SetPos(glm::vec3(pEntityDesc->vPos.x, pEntityDesc->vPos.y, pEntityDesc->vPos.z));    
    oTr.SetRot(glm::quat(glm::vec3(pEntityDesc->vRot.x, pEntityDesc->vRot.y, pEntityDesc->vRot.z)));
    oTr.SetScale(glm::vec3(pEntityDesc->vScl.x, pEntityDesc->vScl.y, pEntityDesc->vScl.z));

    for (int i = 0; i < pEntityDesc->Components.size(); i++)
    {     
      owner_ptr<Component> pComponent = pEntityDesc->Components[i]->GetComponentType()->create().cast_release<Component>();
      pComponent->SetComponentDesc(std::move(pEntityDesc->Components[i]));
      pComponent->Configure();
      pEntity->AddComponent(std::move(pComponent));
    }
  }
}

void SaveScene(const char* _sFilename, const Scene& _rScene)
{
  SceneDesc oSceneDesc;

  for (const owner_ptr<Entity>& pEntity : _rScene.GetEntities())
  {
    owner_ptr<EntityDesc> pEntityDesc = Factory::Create<EntityDesc>();

    const Transform& oTr = pEntity->GetGlobalTransform();
    glm::vec3 vRot = glm::eulerAngles(oTr.GetRot());

    pEntityDesc->vPos.x = oTr.GetPos().x; pEntityDesc->vPos.y = oTr.GetPos().y; pEntityDesc->vPos.z = oTr.GetPos().z;
    pEntityDesc->vRot.x = vRot.x; pEntityDesc->vRot.y = vRot.y; pEntityDesc->vRot.z = vRot.z;
    pEntityDesc->vScl.x = oTr.GetScale().x; pEntityDesc->vScl.y = oTr.GetScale().y; pEntityDesc->vScl.z = oTr.GetScale().z;

    for (const owner_ptr<Component>& pComponent : pEntity->GetComponents())
    {
      const reflection::TypeDescriptor* pCompTypeDesc = pComponent->GetComponentDesc()->GetReflectionDynamic();

      owner_ptr<ComponentDesc> pCompDescCopy = pCompTypeDesc->create().cast_release<ComponentDesc>();

      reflection::CopyTypeVisitor oCopyVisitor(pCompDescCopy.get(), pComponent->GetComponentDesc());
      pCompTypeDesc->Accept(&oCopyVisitor);
      
      pEntityDesc->Components.emplace_back(std::move(pCompDescCopy));
    }

    oSceneDesc.Entities.push_back(std::move(pEntityDesc));
  }

  reflection::ReflectionHelper::Serialize(_sFilename, &oSceneDesc);
}
