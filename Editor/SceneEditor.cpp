#include "SceneEditor.h"

#include "Core/Scene.h"
#include "Graphics/Material.h"
#include "Graphics/Pass.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "Reflection/TypeDescriptors.h"
#include "Reflection/ImGuiTypeVisitor.h"
#include "Reflection/CopyTypeVisitor.h"
#include "Reflection/ReflectionHelper.h"
#include "Core/Engine.h"
#include "Core/SceneLoading.h"
#include "Graphics/MaterialLibrary.h"

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

void ViewMaterial(const std::vector<MaterialLibrary::MatEntry>& _lstMaterials)
{  
  ImGui::Begin("Materials");

  for (auto& rEntry : _lstMaterials)
  {
    if (ImGui::TreeNode(UNIQUE_LABEL("Material", rEntry.m_pMaterial.get())))
    {
      for (const owner_ptr<Pass>& pPass : rEntry.m_pMaterial->GetPasses())
      {
        for (const owner_ptr<ReflectedConstantBuffer>& pCBuff : pPass->GetConstantBuffers())
        {
          ImGui::Text(pCBuff->GetName().c_str());

          for (const ReflectedConstantBuffer::Variable& rVar : pCBuff->GetVariables())
          {
            switch (rVar.m_eType)
            {
            case ConstantBufferType::SCALAR:
            {
              float fValue = 0.f;
              pCBuff->GetFloat(rVar.m_sName.c_str(), &fValue);
              if (ImGui::InputFloat(rVar.m_sName.c_str(), &fValue))
              {
                pCBuff->SetFloat(rVar.m_sName.c_str(), fValue);
              }
            }
              break;
            case ConstantBufferType::VEC4:
            {
              float aValues[4] = { 0.f, 0.f, 0.f, 0.f };
              pCBuff->GetVec4(rVar.m_sName.c_str(), &aValues[0]);
              if (ImGui::InputFloat4(rVar.m_sName.c_str(), aValues))
              {
                pCBuff->SetVec4(rVar.m_sName.c_str(), &aValues[0]);
              }
            }
              break;
            default:
              ImGui::Text(rVar.m_sName.c_str());
            }
          }

          ImGui::Separator();
        }
      }

      if (ImGui::Button(UNIQUE_LABEL("Save", rEntry.m_pMaterial.get())))
      {
        for (const owner_ptr<Pass>& pPass : rEntry.m_pMaterial->GetPasses())
        {
          pPass->UpdateCache();
        }

        const std::string& sFilename = MaterialLibrary::GetInstance()->FindMaterialFilename(rEntry.m_pMaterial.get());
        reflection::ReflectionHelper::Serialize(sFilename.c_str(), rEntry.m_pMaterial.get());
      }

      ImGui::TreePop();
    }    
  }

  ImGui::End();
}
