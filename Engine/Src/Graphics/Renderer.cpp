#include "Graphics/Renderer.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/ConstantBuffer.h"

#include "Math/Transform.h"

#include "GLFW/glfw3.h"

#include <algorithm>

#define KEYPOS_LAST 64

#define GEN_KEY(value, size, pos) static_cast<uint64_t>(value) << (KEYPOS_LAST - size - pos)

struct Renderer::Job
{
  Mesh* m_pMesh;
  const MaterialInstance* m_pMaterial;
  const Window* m_pWindow;
  const Transform* m_pMeshTransform;
  uint64_t m_uKey;

  static bool Compare(const Job& j1, const Job& j2)
  {
    return j1.m_uKey < j2.m_uKey;
  }

  void UpdateRenderKey(Camera* _pCamera, const Transform* _pCamTransform)
  {    
    const RenderStateInfo& rRSInfo = m_pMaterial->GetMaterial()->GetRenderStateInfo();
    bool bTranslucent = rRSInfo.m_bBlendEnabled;

    m_uKey = 0u;    

    // Window id
    m_uKey |= GEN_KEY(m_pWindow->GetId(), 8, KEYPOS_LAST);

    // Translucent? 
    m_uKey |= GEN_KEY(bTranslucent,       1, KEYPOS_LAST - 8);

    // Distance from camera

    float fDist = glm::distance(_pCamTransform->GetPos(), m_pMeshTransform->GetPos());
    float fNormDist = fmax(fmin((fDist - _pCamera->m_fNear) / _pCamera->m_fFar, 1.f), 0.f);

    if (bTranslucent)
    {      
      fNormDist = 1.f - fNormDist;
    }

    uint32_t uDist = static_cast<uint32_t>(4294967295.0f * fNormDist);

    m_uKey |= GEN_KEY(uDist, 32, KEYPOS_LAST - 9);
  }
};

struct Renderer::CamView
{
  Camera* m_pCamera;
  const Transform* m_pTransform;
};

class Renderer::Impl
{  
public:

  Impl() {}

  ~Impl() {}

  std::vector<Job> m_lstJobs;   
  std::vector<CamView> m_lstCamViews;
};

Renderer::Renderer() : m_pImpl(std::make_unique<Impl>())
{

}

Renderer::~Renderer() = default;

void Renderer::Initialize()
{  
   
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  api::InitializeAPI();

}

void Renderer::ShutDown()
{

  glfwTerminate();

  api::ShutDownAPI();
}

void Renderer::SubmitCamera(Camera* _pCamera, const Transform* _pTransform)
{
  m_pImpl->m_lstCamViews.push_back({ _pCamera, _pTransform });
}

void Renderer::SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform)
{    
  m_pImpl->m_lstJobs.push_back({ _pMesh, _pMaterial, _pMesh->GetWindow(), _pTransform, 0u });
}

void Renderer::Draw()
{  

  const Window* pCurrWindow = nullptr;

  const Material* pCurrMaterial = nullptr;

  bool bSkipCurrWindow = false;

  for (CamView& rCamView : m_pImpl->m_lstCamViews)
  {    

    Camera* pCamera = rCamView.m_pCamera;

    // Update key for current camera
    for (Job& rJob : m_pImpl->m_lstJobs)
    {
      rJob.UpdateRenderKey(pCamera, rCamView.m_pTransform);      
    }

    // Sort jobs
    std::sort(m_pImpl->m_lstJobs.begin(), m_pImpl->m_lstJobs.end(), Job::Compare);

    // Perform jobs
    for (Job& rJob : m_pImpl->m_lstJobs)
    {

      if (rJob.m_pWindow != pCamera->GetWindow())
      {
        continue;
      }

      if (pCurrWindow != rJob.m_pWindow)
      {
        if (pCurrWindow != nullptr && !bSkipCurrWindow)
        {
          pCurrWindow->EndDraw();
        }
        bSkipCurrWindow = rJob.m_pWindow->BeginDraw() != 0;        
        pCurrWindow = rJob.m_pWindow;
      }

      if (!bSkipCurrWindow)
      {
        const MaterialInstance* pMatInstance = rJob.m_pMaterial;

        const Material* pMaterial = pMatInstance->GetMaterial();

        if (pMaterial != pCurrMaterial)
        {
          pMaterial->Bind();
          pCamera->Bind();
          pCurrMaterial = pMaterial;
        }

        rJob.m_pMesh->UpdateTransform(*rJob.m_pMeshTransform);

        pMatInstance->Bind();

        rJob.m_pMesh->Draw();
      }
    }

    if (pCurrWindow != nullptr && !bSkipCurrWindow)
    {
      pCurrWindow->EndDraw();
    }
  }

  m_pImpl->m_lstJobs.clear();
  m_pImpl->m_lstCamViews.clear();
}
