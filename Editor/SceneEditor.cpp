#include "SceneEditor.h"

#include "Core/Scene.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "Reflection/TypeDescriptors.h"
#include "Reflection/ImGuiTypeVisitor.h"
#include "Reflection/CopyTypeVisitor.h"
#include "Core/Engine.h"
#include "Core/SceneLoading.h"

#define UNIQUE_LABEL(LABEL, PTR) (std::string(LABEL) + "##" + std::to_string((size_t)PTR)).c_str()

void ViewScene(Scene* _pScene)
{    
  ImGui::Begin("Scene");
  
  static Scene* pLastScene = nullptr;
  static char aBuff[512];

  if (pLastScene != _pScene)
  {
    memcpy(aBuff, _pScene->m_sFilename.c_str(), _pScene->m_sFilename.size());
    aBuff[_pScene->m_sFilename.size()] = 0;
    pLastScene = _pScene;
  }
  
  ImGui::InputText("Path", aBuff, 512);

  bool bSaveScene = ImGui::Button("Save scene");
  ImGui::SameLine();
  bool bLoadScene = ImGui::Button("Load scene");
  if (bSaveScene)
  {
    SaveScene(aBuff, *_pScene);
  }
  else if (bLoadScene)
  {
    Engine::GetInstance()->ScheduleSceneLoad(aBuff);
  }

  ImGui::TreePush("Entities");

  for (owner_ptr<Entity>& pEntity : _pScene->GetEntities())
  {        
    if (ImGui::TreeNode(UNIQUE_LABEL("Entity", pEntity.get())))
    {
      Transform& rTr = pEntity->GetMutableLocalTransform();

      glm::vec3 vPos = rTr.GetPos();
      if (ImGui::InputFloat3(UNIQUE_LABEL("Pos", pEntity.get()), &vPos.x))
      {
        rTr.SetPos(vPos);
      }
      
      glm::vec3 vRot = glm::eulerAngles(rTr.GetRot());
      if (ImGui::InputFloat3(UNIQUE_LABEL("Rot", pEntity.get()), &vRot.x))
      {
        rTr.SetRot(glm::quat(vRot));
      }

      glm::vec3 vScl = rTr.GetScale();
      if (ImGui::InputFloat3(UNIQUE_LABEL("Scl", pEntity.get()), &vScl.x))
      {
        rTr.SetScale(vScl);
      }

      if (ImGui::TreeNode(UNIQUE_LABEL("Components", pEntity.get())))
      {
        for (const owner_ptr<Component>& pComponent : pEntity->GetComponents())
        {             
          ComponentDesc* pCompDesc  = pComponent->GetComponentDesc();
          ImGuiTypeVisitor oImGuiVisitor(static_cast<void*>(pCompDesc), pComponent->GetReflectionDynamic()->getFullName().c_str());
          pCompDesc->GetReflectionDynamic()->Accept(&oImGuiVisitor);
        }
        ImGui::TreePop();
      }

      ImGui::TreePop();
    }
  }

  ImGui::TreePop();  

  ImGui::End();
}