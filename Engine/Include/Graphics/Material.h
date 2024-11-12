#pragma once

#include <vector>
#include <memory>

#include "Graphics/Pass.h"

class Resource;

class Material
{
public:

  Material(std::vector<pooled_ptr<Pass>>&& _lstPasses);

  Material(Material&& rMaterial);  

  ~Material();

  void Setup() const;  

  const std::vector<pooled_ptr<Pass>>& GetPasses() { return m_lstPasses; }

private:
  
  std::vector<pooled_ptr<Pass>> m_lstPasses;
  
};