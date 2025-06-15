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
    uint32_t m_uArraySize;
    std::string m_sName;
  };

  ~ReflectedConstantBuffer();

  void Configure(std::string _sName, PipelineStage _eStage, std::vector<Variable>&& _lstVariables, float* _pCache);

  void Update() const
  {
    ConstantBufferBase::Update(m_pData, m_uSize);
  }

  const std::string& GetName() { return m_sName; }  

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

      size_t uVarSize = GetConstantBufferTypeSize(rVar.m_eType) * m_lstVariables[i].m_uArraySize;

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

      size_t uVarSize = GetConstantBufferTypeSize(rVar.m_eType) * m_lstVariables[i].m_uArraySize;

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

  size_t GetSize() const override { return m_uSize; }

  const std::string& GetName() const { return m_sName; }

  PipelineStage GetPipelineStage() const { return m_eStage; }

private:

  void* m_pData = nullptr;

  std::vector<Variable> m_lstVariables;

  size_t m_uSize = 0u;

  std::string m_sName;

  PipelineStage m_eStage;

};
