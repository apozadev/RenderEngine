#include "Graphics/Mesh.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/API/GraphicsAPI.h"

class Mesh::Impl
{
public:
  api::APIMesh* m_pAPIMesh;
  Window* m_pWindow;
  uint32_t m_uVertexCount;
};

Mesh::Mesh(std::vector<Vertex>& _lstVertices, Window* _pWindow, ConstructKey&&)
{
  m_pImpl = new Impl{};

  m_pImpl->m_pWindow = _pWindow;
  m_pImpl->m_uVertexCount = _lstVertices.size();

  Renderer::GetInstance()->SetUsingWindow(_pWindow);
  m_pImpl->m_pAPIMesh = api::CreateAPIMesh(_lstVertices.data(), m_pImpl->m_uVertexCount * sizeof(Vertex));
}

Mesh::~Mesh()
{
  delete(m_pImpl);
}

Window* Mesh::GetWindow() const  
{
  return m_pImpl->m_pWindow;
}

uint64_t Mesh::GetKey() const
{
  return static_cast<uint64_t>(m_pImpl->m_pWindow->GetId()) << (sizeof(uint64_t) - sizeof(uint8_t));
}

void Mesh::Draw() const
{
  api::DrawMesh(m_pImpl->m_pAPIMesh, m_pImpl->m_uVertexCount);
}
