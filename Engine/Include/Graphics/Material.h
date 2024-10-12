#pragma once

#include <vector>
#include <memory>

#include "Graphics/Pass.h"

class Resource;

class Material
{
public:

  Material(std::vector<Pass>&& _lstPasses);

  Material(Material&& rMaterial);  

  void Setup() const;  

  const std::vector<Pass>& GetPasses() { return m_lstPasses; }

private:
  
  std::vector<Pass> m_lstPasses;
  
};