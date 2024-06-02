#pragma once

#include <string>
#include <memory>

#include "Graphics/Resource.h"

class Material;

class Texture2D : public Resource
{
public:
  //Texture2D() {};
  Texture2D(const std::string& _sFilename, int _iBinding, PipelineStage _eStage);

  virtual ~Texture2D() = default;

  void Setup(ResourceFrequency _eFrequency) const override;

  void Bind() const override;

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;

};