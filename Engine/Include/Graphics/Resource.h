#pragma once

class Resource
{
public:
  virtual void Bind() const = 0;

  virtual void Setup() const = 0;

  virtual ~Resource() {};
};