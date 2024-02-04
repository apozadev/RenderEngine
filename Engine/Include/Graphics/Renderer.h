#include "Core/Singleton.h"

class Renderer : public core::Singleton<Renderer>
{
public:
  void Initialize();
  void ShutDown();
private:
};