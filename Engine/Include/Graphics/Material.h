#pragma once

#include <vector>
#include <memory>

#include "Graphics/Pass.h"
#include "Memory/PtrTypes.h"
#include "Core/BaseObject.h"

class Resource;

class Material : public BaseObject
{
public:

  ~Material();  

  void AddPass(owner_ptr<Pass>&& _pPass);

  void Setup() const;  

  const std::vector<owner_ptr<Pass>>& GetPasses() { return m_lstPasses; }

private:
  
  std::vector<owner_ptr<Pass>> m_lstPasses;
  
};