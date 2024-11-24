#pragma once

#include <vector>
#include <memory>

#include "Core/PooledObject.h"
#include "Graphics/Pass.h"

class Resource;

class Material : public TypedPooledObject<Material, 256>
{
public:

  using TypedPooledObject<Material, 256>::TypedPooledObject;

  ~Material();  

  void AddPass(pooled_ptr<Pass>&& _pPass);

  void Setup() const;  

  const std::vector<pooled_ptr<Pass>>& GetPasses() { return m_lstPasses; }

private:
  
  std::vector<pooled_ptr<Pass>> m_lstPasses;
  
};