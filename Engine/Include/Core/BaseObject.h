#pragma once

class BaseObject {
public:
  virtual void Setup() {};
  virtual void Reconfigure() {};
private:
  bool m_initialized = false;
};