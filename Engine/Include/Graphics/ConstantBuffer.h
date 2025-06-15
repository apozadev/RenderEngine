#pragma once

#include <string>

#include "Graphics/API/GraphicsAPI.h"
#include "Core/BaseObject.h"

class ConstantBufferBase : public BaseObject
{

public:
  
  virtual ~ConstantBufferBase();  

  void Bind() const;

  virtual size_t GetSize() const = 0;

protected:  

  void Configure(size_t _uSize);

  void Update(const void* _pData, size_t _uSize) const;

public:

  api::APIConstantBuffer* m_pAPICbuffer;
};

template <typename T>
class ConstantBuffer : public ConstantBufferBase
{  
public:      

  void Configure()
  {
    ConstantBufferBase::Configure(sizeof(T));
  }

  void Update() const
  {
    ConstantBufferBase::Update(&m_oData, sizeof(T));
  }

  T* GetData()
  {
    return &m_oData;
  }

  void SetData(T* _pData)
  {
    m_oData = *_pData;
  }    

  size_t GetSize() const override { return sizeof(T); }

protected:

  T m_oData;
};