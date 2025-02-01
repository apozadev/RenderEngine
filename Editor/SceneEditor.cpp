#include "SceneEditor.h"

#include "Core/Scene.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#define UNIQUE_LABEL(LABEL, PTR) (std::string(LABEL) + "##" + std::to_string((size_t)PTR)).c_str()

void ViewScene(Scene* _pScene)
{  

  ImGui::Begin("Scene");

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

      ImGui::TreePop();
    }
  }

  ImGui::TreePop();

  ImGui::End();
}