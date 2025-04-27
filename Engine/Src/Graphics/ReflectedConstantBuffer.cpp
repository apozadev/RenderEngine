#include "Graphics/ReflectedConstantBuffer.h"

ReflectedConstantBuffer::~ReflectedConstantBuffer()
{
  if (m_pData != nullptr)
  {
    free(m_pData);
  }
}

void ReflectedConstantBuffer::Configure(std::string _sName, PipelineStage _eStage, std::vector<Variable>&& _lstVariables, float* _pCache)
{

  m_sName = _sName;

  m_eStage = _eStage;

  m_uSize = 0u;

  for (unsigned int i = 0u; i < _lstVariables.size(); i++)
  {    
    size_t uVarSize = GetConstantBufferTypeSize(_lstVariables[i].m_eType) * _lstVariables[i].m_uArraySize;

    // padding?
    if (i < _lstVariables.size() - 1u)
    {
      size_t uNextSize = GetConstantBufferTypeSize(_lstVariables[i + 1u].m_eType);
      if (uVarSize % uNextSize != 0u) uVarSize += uNextSize - uVarSize % uNextSize;
    }

    m_uSize += uVarSize;
  }  

  size_t uOgSize = m_uSize;

  if (m_uSize % 16u != 0u) m_uSize += 16u - m_uSize % 16u;

  m_pData = malloc(m_uSize);
  memset(m_pData, 0, m_uSize);

  // Apply cache
  if (_pCache)
  {
    size_t uCacheIdx = 0u;
    size_t uDataIdx = 0u;
    for (unsigned int i = 0u; i < _lstVariables.size(); i++)
    {
      size_t uVarSize = GetConstantBufferTypeSize(_lstVariables[i].m_eType) * _lstVariables[i].m_uArraySize;
      size_t uPaddedVarSize = uVarSize;
      // padding?
      if (i < _lstVariables.size() - 1u)
      {
        size_t uNextSize = GetConstantBufferTypeSize(_lstVariables[i + 1u].m_eType);
        if (uVarSize % uNextSize != 0u) uPaddedVarSize += uNextSize - uVarSize % uNextSize;
      }

      memcpy(((char*)m_pData) + uDataIdx, _pCache + uCacheIdx, uVarSize);

      uDataIdx += uPaddedVarSize;
      uCacheIdx += uVarSize / sizeof(float);
    }
  }

  m_lstVariables = std::move(_lstVariables);

  ConstantBufferBase::Configure(m_uSize);

}
