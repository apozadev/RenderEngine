#include "Graphics/ReflectedConstantBuffer.h"

ReflectedConstantBuffer::~ReflectedConstantBuffer()
{
  if (m_pData != nullptr)
  {
    free(m_pData);
  }
}

void ReflectedConstantBuffer::Configure(std::string _sName, std::vector<Variable>&& _lstVariables)
{

  m_sName = _sName;

  m_uSize = 0u;

  for (Variable& rVar : _lstVariables)
  {
    m_uSize += GetConstantBufferTypeSize(rVar.m_eType);
  }  

  if (m_uSize % 16u != 0u) m_uSize += 16u - m_uSize % 16u;

  m_pData = malloc(m_uSize);

  m_lstVariables = std::move(_lstVariables);

  ConstantBufferBase::Configure(m_uSize);

}
