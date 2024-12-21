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

  void Configure(std::string _sName, std::vector<Variable>&& _lstVariables);

  void SetupRenderSubState() const
  {
    ConstantBufferBase::SetupRenderSubState(m_uSize, m_sName, PipelineStage::PIXEL, ResourceFrequency::MATERIAL);
  }

  void Update() const
  {
    ConstantBufferBase::Update(m_pData, m_uSize);
  }

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
    for (Variable& rVar : m_lstVariables)
    {
      size_t uVarSize = GetConstantBufferTypeSize(rVar.m_eType);

      if (rVar.m_eType == TYPE && rVar.m_sName == _sName)
      {                
        memcpy(static_cast<char*>(m_pData) + uOffset, _pData, uVarSize);
        return true;        
      }

      uOffset += uVarSize;
    }

    return false;
  }

  template<ConstantBufferType TYPE>
  bool GetVar(const char* _sName, float* pOutValue_) const 
  {
    size_t uOffset = 0u;
    for (const Variable& rVar : m_lstVariables)
    {
      size_t uVarSize = GetConstantBufferTypeSize(rVar.m_eType);

      if (rVar.m_eType == TYPE && rVar.m_sName == _sName)
      {
        memcpy(pOutValue_, static_cast<char*>(m_pData) + uOffset, uVarSize);
        return true;
      }

      uOffset += uVarSize;
    }

    return false;
  }

protected:

  void SetupRenderSubState(const std::string& _sName, PipelineStage _eStage, ResourceFrequency _eFrequency) const override
  {
    ConstantBufferBase::SetupRenderSubState(m_uSize, _sName, _eStage, _eFrequency);
  }

private:

  void* m_pData = nullptr;

  std::vector<Variable> m_lstVariables;

  size_t m_uSize = 0u;

  std::string m_sName;

};
