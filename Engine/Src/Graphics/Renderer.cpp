#include "Graphics/Renderer.h"
#include "Graphics/API/GraphicsAPI.h"

void Renderer::Initialize()
{
  api::InitializeAPI();
}

void Renderer::ShutDown()
{
  api::ShutDownAPI();
}
