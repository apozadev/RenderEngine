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

  for (unsigned int i = 0u; i < _lstVariables.size(); i++)
  {    
    size_t uVarSize = GetConstantBufferTypeSize(_lstVariables[i].m_eType);

    // padding?
    if (i < _lstVariables.size() - 1u)
    {
      size_t uNextSize = GetConstantBufferTypeSize(_lstVariables[i + 1u].m_eType);
      if (uVarSize % uNextSize != 0u) uVarSize += uNextSize - uVarSize % uNextSize;
    }

    m_uSize += uVarSize;
  }  

  if (m_uSize % 16u != 0u) m_uSize += 16u - m_uSize % 16u;

  m_pData = malloc(m_uSize);

  m_lstVariables = std::move(_lstVariables);

  ConstantBufferBase::Configure(m_uSize);

}
