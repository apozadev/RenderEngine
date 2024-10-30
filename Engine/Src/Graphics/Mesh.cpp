#include "Graphics/Mesh.h"
#include "Graphics/Window.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/API/GraphicsAPI.h"

class Mesh::Impl
{
public:
  api::APIMesh* m_pAPIMesh;
  glm::mat4 m_mLocalTransform;
  uint32_t m_uVertexCount;
  uint32_t m_uIndexCount;  
  MeshConstant m_oConstant;

  Impl(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices)
  {    
    m_uVertexCount = _lstVertices.size();
    m_uIndexCount = _lstIndices.size();
    m_mLocalTransform = glm::mat4(1.f);        
    
    m_pAPIMesh = api::CreateAPIMesh(_lstVertices.data(), m_uVertexCount * sizeof(Vertex), _lstIndices.data(), m_uIndexCount * sizeof(uint16_t));

  }

  ~Impl()
  {
    api::DestroyAPIMesh(m_pAPIMesh);
  }
};

Mesh::Mesh(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices)
{
  m_pImpl = std::make_unique<Impl>(_lstVertices, _lstIndices);  
}

Mesh::Mesh(Mesh&& _rMesh) : m_pImpl(std::move(_rMesh.m_pImpl))
{
}

Mesh::~Mesh()
{  
}

void Mesh::UpdateTransform(const Transform& _oParentTransform)
{    
  glm::mat4 mParentMat = _oParentTransform.GetMatrix();  
  m_pImpl->m_oConstant.m_mModel = mParentMat * m_pImpl->m_mLocalTransform;
  m_pImpl->m_oConstant.m_mNormal = glm::transpose(glm::inverse(m_pImpl->m_oConstant.m_mModel));
}

void Mesh::Draw() const
{  
  api::DrawMesh(m_pImpl->m_pAPIMesh, m_pImpl->m_uIndexCount, &m_pImpl->m_oConstant, sizeof(MeshConstant));
}
