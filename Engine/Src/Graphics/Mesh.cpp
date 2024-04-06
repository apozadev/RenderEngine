#include "Graphics/Mesh.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/API/GraphicsAPI.h"

class Mesh::Impl
{
public:
  api::APIMesh* m_pAPIMesh;
  Window* m_pWindow;
  ConstantBuffer<glm::mat4>* m_pTransformCBuffer;
  glm::mat4 m_mLocalTransform;
  uint32_t m_uVertexCount;
  uint32_t m_uIndexCount;    

  Impl(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, MaterialInstance* _pMaterial, Window* _pWindow)
  {
    m_pWindow = _pWindow;
    m_uVertexCount = _lstVertices.size();
    m_uIndexCount = _lstIndices.size();
    m_mLocalTransform = glm::mat4(1.f);
    m_pTransformCBuffer = _pMaterial->AddResource<ConstantBuffer<glm::mat4>>();

    _pWindow->SetUsing();
    m_pAPIMesh = api::CreateAPIMesh(_lstVertices.data(), m_uVertexCount * sizeof(Vertex), _lstIndices.data(), m_uIndexCount * sizeof(uint16_t));
  }

  ~Impl()
  {
    api::DestroyAPIMesh(m_pAPIMesh);
  }
};

Mesh::Mesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices, MaterialInstance* _pMaterial, Window* _pWindow/*, ConstructKey&&*/)
{
  m_pImpl = std::make_unique<Impl>(_lstVertices, _lstIndices, _pMaterial, _pWindow);  
}

Mesh::Mesh(Mesh&& _rMesh) : m_pImpl(std::move(_rMesh.m_pImpl))
{
}

Mesh::~Mesh()
{  
}

Window* Mesh::GetWindow() const  
{
  return m_pImpl->m_pWindow;
}

uint64_t Mesh::GetKey() const
{
  return static_cast<uint64_t>(m_pImpl->m_pWindow->GetId()) << (sizeof(uint64_t) - sizeof(uint8_t));
}

void Mesh::UpdateTransform(const Transform& _oParentTransform)
{    
  /*(*pMat)[0] = mMat[0][0]; (*pMat)[1] = mMat[0][1]; (*pMat)[2] = mMat[0][2]; (*pMat)[3] = mMat[0][3];
  (*pMat)[4] = mMat[1][0]; (*pMat)[5] = mMat[1][1]; (*pMat)[6] = mMat[1][2]; (*pMat)[7] = mMat[1][3];
  (*pMat)[8] = mMat[2][0]; (*pMat)[9] = mMat[2][1]; (*pMat)[10]= mMat[2][2]; (*pMat)[11]= mMat[2][3];
  (*pMat)[12]= mMat[3][0]; (*pMat)[13]= mMat[3][1]; (*pMat)[14]= mMat[3][2]; (*pMat)[15]= mMat[3][3];*/
  
  //glm::mat4x4* pGlobalMat = m_pImpl->m_pTransformCBuffer->GetData();
  glm::mat4 mParentMat = _oParentTransform.GetMatrix();
  *m_pImpl->m_pTransformCBuffer->GetData() = mParentMat * m_pImpl->m_mLocalTransform;
  m_pImpl->m_pTransformCBuffer->Update();
}

void Mesh::Draw()
{  

  m_pImpl->m_pTransformCBuffer->Bind();

  api::DrawMesh(m_pImpl->m_pAPIMesh, m_pImpl->m_uIndexCount);
}
