#pragma once

#include <vector>
#include <memory>

#include "Graphics/Pass.h"
#include "Memory/PtrTypes.h"
#include "Core/BaseObject.h"
#include "Reflection/ReflectionMacros.h"

class Resource;

class Material : public BaseObject
{
public:

  REFLECT_BASE()

  ~Material();  

  void AddPass(owner_ptr<Pass>&& _pPass);

  void Setup() const;  

  const std::vector<owner_ptr<Pass>>& GetPasses() const { return m_lstPasses; }

private:
  
  std::vector<owner_ptr<Pass>> m_lstPasses;
  
};
