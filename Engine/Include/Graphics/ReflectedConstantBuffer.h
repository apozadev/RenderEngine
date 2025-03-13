#pragma once

#include <string>
#include <vector>

#include "Graphics/ConstantBufferTypes.h"
#include "Graphics/ConstantBuffer.h"

class ReflectedConstantBuffer : public ConstantBufferBase
{
public:

  struct Variable
  {
    ConstantBufferType m_eType;
    std::string m_sName;
  };

  ~ReflectedConstantBuffer();

  void Configure(std::string _sName, std::vector<Variable>&& _lstVariables, float* _pCache);

  void SetupRenderSubState() const
  {
    ConstantBufferBase::SetupRenderSubState(m_uSize, m_sName, STAGE_PIXEL, ResourceFrequency::MATERIAL);
  }

  void Update() const
  {
    ConstantBufferBase::Update(m_pData, m_uSize);
  }

  const std::string& GetName() { return m_sName; }

  size_t GetSize() const { return m_uSize; }

  const std::vector<Variable>& GetVariables() { return m_lstVariables; }

  bool SetFloat(const char* _sName, float _fValue) { return SetVar<ConstantBufferType::SCALAR>(_sName, &_fValue); }
  bool SetVec2(const char* _sName, float* _pData)  { return SetVar<ConstantBufferType::VEC2>(_sName, _pData); }
  bool SetVec3(const char* _sName, float* _pData)  { return SetVar<ConstantBufferType::VEC3>(_sName, _pData); }
  bool SetVec4(const char* _sName, float* _pData)  { return SetVar<ConstantBufferType::VEC4>(_sName, _pData); }
  bool SetMat3(const char* _sName, float* _pData)  { return SetVar<ConstantBufferType::MAT3>(_sName, _pData); }
  bool SetMat4(const char* _sName, float* _pData)  { return SetVar<ConstantBufferType::MAT4>(_sName, _pData); }

  bool GetFloat(const char* _sName, float* pOutValue_) const { return GetVar<ConstantBufferType::SCALAR>(_sName, pOutValue_); }
  bool GetVec2(const char* _sName, float* pOutValue_)  const { return GetVar<ConstantBufferType::VEC2>(_sName, pOutValue_); }
  bool GetVec3(const char* _sName, float* pOutValue_)  const { return GetVar<ConstantBufferType::VEC3>(_sName, pOutValue_); }
  bool GetVec4(const char* _sName, float* pOutValue_)  const { return GetVar<ConstantBufferType::VEC4>(_sName, pOutValue_); }
  bool GetMat3(const char* _sName, float* pOutValue_)  const { return GetVar<ConstantBufferType::MAT3>(_sName, pOutValue_); }
  bool GetMat4(const char* _sName, float* pOutValue_)  const { return GetVar<ConstantBufferType::MAT4>(_sName, pOutValue_); }

  template<ConstantBufferType TYPE>
  bool SetVar(const char* _sName, float* _pData)
  {
    size_t uOffset = 0u;

    for (unsigned int i = 0u; i < m_lstVariables.size(); i++)
    {
      const Variable& rVar = m_lstVariables[i];

      size_t uVarSize = GetConstantBufferTypeSize(rVar.m_eType);      

      if (rVar.m_eType == TYPE && rVar.m_sName == _sName)
      {
        memcpy(static_cast<char*>(m_pData) + uOffset, _pData, uVarSize);
        return true;
      }

      // padding?
      if (i < m_lstVariables.size() - 1u)
      {
        size_t uNextSize = GetConstantBufferTypeSize(m_lstVariables[i + 1u].m_eType);
        if (uVarSize % uNextSize != 0u) uVarSize += uNextSize - uVarSize % uNextSize;
      }

      uOffset += uVarSize;
    }

    return false;
  }

  template<ConstantBufferType TYPE>
  bool GetVar(const char* _sName, float* pOutValue_) const 
  {
    size_t uOffset = 0u;
    for (unsigned int i = 0u; i < m_lstVariables.size(); i++)
    {
      const Variable& rVar = m_lstVariables[i];

      size_t uVarSize = GetConstantBufferTypeSize(rVar.m_eType);

      if (rVar.m_eType == TYPE && rVar.m_sName == _sName)
      {
        memcpy(pOutValue_, static_cast<char*>(m_pData) + uOffset, uVarSize);
        return true;
      }

      // padding?
      if (i < m_lstVariables.size() - 1u)
      {
        size_t uNextSize = GetConstantBufferTypeSize(m_lstVariables[i + 1u].m_eType);
        if (uVarSize % uNextSize != 0u) uVarSize += uNextSize - uVarSize % uNextSize;
      }

      uOffset += uVarSize;
    }

    return false;
  }

protected:

  void SetupRenderSubState(const std::string& _sName, PipelineStageFlags _uStageFlags, ResourceFrequency _eFrequency) const override
  {
    ConstantBufferBase::SetupRenderSubState(m_uSize, _sName, _uStageFlags, _eFrequency);
  }

private:

  void* m_pData = nullptr;

  std::vector<Variable> m_lstVariables;

  size_t m_uSize = 0u;

  std::string m_sName;

};
