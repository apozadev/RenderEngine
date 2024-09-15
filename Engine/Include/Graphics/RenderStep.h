#pragma once

#include <memory>
#include <vector>

class Window;
class RenderTarget;

class RenderStep
{
public:

  RenderStep(std::vector<RenderTarget*>&& _lstInputs, const RenderTarget* _pRenderTarget);

  RenderStep(RenderStep&& _rOther);

  ~RenderStep();

  void Bind(const Window* _pWindow) const;

private:

  class Impl;

  std::unique_ptr<Impl> m_pImpl;  
};
