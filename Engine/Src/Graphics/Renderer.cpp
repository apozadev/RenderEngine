#include "Graphics/Renderer.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Mesh.h"
#include "Graphics/Window.h"

#include "Math/Transform.h"

#include "GLFW/glfw3.h"

#include <algorithm>

bool Renderer::compareJob(const Job& j1, const Job& j2)
{
  return j1.m_uKey < j2.m_uKey;
}

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

Window* Renderer::CreateNewWindow(int _iWidth, int _iHeight, const char* _sTitle)
{
  Window* pWindow = new Window(_iWidth, _iHeight, _sTitle, Window::ConstructKey());
  m_lstWindows.push_back(pWindow);
  return pWindow;
}

void Renderer::SetUsingWindow(Window* _pWindow)
{
  _pWindow->SetUsing();
}

Mesh* Renderer::CreateMesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, Window* _pWindow)
{
  return new Mesh(_lstVertices, _lstIndices, _pWindow, Mesh::ConstructKey());
}

void Renderer::SubmitMesh(Mesh* _pMesh)
{
  m_lstJobs.push_back({ _pMesh, _pMesh->GetWindow(), _pMesh->GetKey()});
}

void Renderer::UpdateWindows()
{
  size_t uCurrSize = m_lstWindows.size();
  for (int i = 0; i < uCurrSize; i++)
  {
    if (m_lstWindows[i]->ShouldClose())
    {
      if (i < uCurrSize - 1)
      {
        delete(m_lstWindows[i]);
        m_lstWindows[i] = m_lstWindows[m_lstWindows.size() - 1];
      }
      --uCurrSize;
    }
  }
  m_lstWindows.resize(uCurrSize);
}

Transform s_oTransform;

void Renderer::Draw()
{  

  std::sort(m_lstJobs.begin(), m_lstJobs.end(), compareJob);

  Window* pCurrWindow = nullptr;

  bool bSkipCurrWindow = false;

  s_oTransform.Translate({0.01f, 0.f, 0.f});

  for (Job& rJob : m_lstJobs)
  {
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
      rJob.m_pMesh->Draw(s_oTransform);
    }
  }

  if (pCurrWindow != nullptr && !bSkipCurrWindow)
  {
    pCurrWindow->EndDraw();
  }

  m_lstJobs.clear();
}
