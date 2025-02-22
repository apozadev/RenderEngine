#pragma once

class BaseObject {
public:
  virtual void Configure() {};
  virtual void Reconfigure() {};
private:
  bool m_initialized = false;
};