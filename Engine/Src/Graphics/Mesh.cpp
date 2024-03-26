#include "Graphics/Mesh.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"



class Mesh::Impl
{
public:
  api::APIMesh* m_pAPIMesh;
  Window* m_pWindow;
  ConstantBuffer<float[16]> m_oTransformCBuffer;
  uint32_t m_uVertexCount;
  uint32_t m_uIndexCount;    
};

Mesh::Mesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, Window* _pWindow, ConstructKey&&)
{
  m_pImpl = new Impl{};

  m_pImpl->m_pWindow = _pWindow;
  m_pImpl->m_uVertexCount = _lstVertices.size();
  m_pImpl->m_uIndexCount = _lstIndices.size();    

  Renderer::GetInstance()->SetUsingWindow(_pWindow);
  m_pImpl->m_pAPIMesh = api::CreateAPIMesh(_lstVertices.data(), m_pImpl->m_uVertexCount * sizeof(Vertex), _lstIndices.data(), m_pImpl->m_uIndexCount * sizeof(uint16_t));
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

void Mesh::Draw(const Transform& _oParentTransform)
{
  float (*pMat) [16] =  m_pImpl->m_oTransformCBuffer.GetData();
  const glm::mat4x4& mMat = _oParentTransform.GetMatrix();
  (*pMat)[0] = mMat[0][0]; (*pMat)[1] = mMat[0][1]; (*pMat)[2] = mMat[0][2]; (*pMat)[3] = mMat[0][3];
  (*pMat)[4] = mMat[1][0]; (*pMat)[5] = mMat[1][1]; (*pMat)[6] = mMat[1][2]; (*pMat)[7] = mMat[1][3];
  (*pMat)[8] = mMat[2][0]; (*pMat)[9] = mMat[2][1]; (*pMat)[10]= mMat[2][2]; (*pMat)[11]= mMat[2][3];
  (*pMat)[12]= mMat[3][0]; (*pMat)[13]= mMat[3][1]; (*pMat)[14]= mMat[3][2]; (*pMat)[15]= mMat[3][3];

  m_pImpl->m_oTransformCBuffer.Bind();

  api::DrawMesh(m_pImpl->m_pAPIMesh, m_pImpl->m_uIndexCount);
}
