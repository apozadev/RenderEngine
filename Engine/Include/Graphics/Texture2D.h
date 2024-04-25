#pragma once

#include <string>
#include <memory>

#include "Graphics/Resource.h"

class Material;

class Texture2D : public Resource
{
public:
  //Texture2D() {};
  Texture2D(const std::string& _sFilename);

  virtual ~Texture2D() = default;

  void Setup() const override;

  void Bind() const override;

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;

};