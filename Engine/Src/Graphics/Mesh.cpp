#include "Graphics/Mesh.h"
#include "Graphics/Window.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/MaterialInstance.h"
#include "Graphics/API/GraphicsAPI.h"

#include "Reflection/ReflectionImplMacros.h"

void Mesh::Initialize(std::vector<Vertex>& _lstVertices, std::vector<uint16_t>& _lstIndices)
{
  m_uVertexCount = static_cast<uint32_t>(_lstVertices.size());
  m_uIndexCount = static_cast<uint32_t>(_lstIndices.size());
  m_mLocalTransform = glm::mat4(1.f);

  m_pAPIMesh = api::CreateAPIMesh(_lstVertices.data(), m_uVertexCount * sizeof(Vertex), _lstIndices.data(), m_uIndexCount * sizeof(uint16_t));
}

Mesh::~Mesh()
{
  api::DestroyAPIMesh(m_pAPIMesh);
}

void Mesh::UpdateTransform(const Transform& _oParentTransform)
{    
  glm::mat4 mParentMat = _oParentTransform.GetMatrix();  
  m_oConstant.m_mModel = mParentMat * m_mLocalTransform;
  m_oConstant.m_mNormal = glm::transpose(glm::inverse(m_oConstant.m_mModel));
}

void Mesh::Draw() const
{  
  api::DrawMesh(m_pAPIMesh, m_uIndexCount, &m_oConstant, sizeof(MeshConstant));
}

REFLECT_STRUCT_BASE_BEGIN(Mesh)
REFLECT_STRUCT_MEMBER(m_uIndexCount)
REFLECT_STRUCT_MEMBER(m_uVertexCount)
REFLECT_STRUCT_END(Mesh)

IMPLEMENT_REFLECTION_POINTER(Mesh)
