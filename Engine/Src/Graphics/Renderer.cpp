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

struct Renderer::Job
{
  Mesh* m_pMesh;
  const MaterialInstance* m_pMaterial;
  const Window* m_pWindow;
  const Transform* m_pTransform;
  uint64_t m_uKey;

  static bool Compare(const Job& j1, const Job& j2)
  {
    return j1.m_uKey < j2.m_uKey;
  }
};

class Renderer::Impl
{  
public:

  Impl() {}

  ~Impl() {}

  std::vector<Job> m_lstJobs;   
  std::vector<Camera*> m_lstCameras;
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

void Renderer::SubmitCamera(Camera* _pCamera)
{
  m_pImpl->m_lstCameras.push_back(_pCamera);
}

void Renderer::SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform)
{
  m_pImpl->m_lstJobs.push_back({ _pMesh, _pMaterial, _pMesh->GetWindow(), _pTransform, _pMesh->GetKey()});
}

void Renderer::Draw()
{  

  std::sort(m_pImpl->m_lstJobs.begin(), m_pImpl->m_lstJobs.end(), Job::Compare);

  const Window* pCurrWindow = nullptr;

  const Material* pCurrMaterial = nullptr;

  bool bSkipCurrWindow = false;

  for (Camera* pCamera : m_pImpl->m_lstCameras)
  {    

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
        pCamera->Bind();
        pCurrWindow = rJob.m_pWindow;
      }

      if (!bSkipCurrWindow)
      {
        const MaterialInstance* pMatInstance = rJob.m_pMaterial;

        const Material* pMaterial = pMatInstance->GetMaterial();

        if (pMaterial != pCurrMaterial)
        {
          pMaterial->Bind();
          pCurrMaterial = pMaterial;
        }

        rJob.m_pMesh->UpdateTransform(*rJob.m_pTransform);

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
  m_pImpl->m_lstCameras.clear();
}
