#pragma once

#include <memory>
#include <string>

#include "Core/Singleton.h"

class Mesh;
class MaterialInstance;
class Transform;
class Window;
class Camera;
class RenderPipeline;
struct RenderPipelineConfig;

class Renderer : public Singleton<Renderer>
{

public:  

  Renderer();
  ~Renderer();

  void Initialize();
  void ShutDown();   

  void AddRenderPipeline(const Window* _pWindow, RenderPipelineConfig&& _pPipelineCofig);  

  void SubmitCamera(Camera* _pCamera, const Transform* _pTransform);
  void SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform);

  void OnWindowResize(const Window* _pWindow);
  
  void Draw();

private:  

  class Impl;
  std::unique_ptr<Impl> m_pImpl;    
};