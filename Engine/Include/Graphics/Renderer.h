#pragma once

#include <memory>

#include "Core/Singleton.h"

class Mesh;
class MaterialInstance;
class Transform;
class Window;
class Camera;

class Renderer : public Singleton<Renderer>
{

  struct Job;

public:  

  Renderer();
  ~Renderer();

  void Initialize();
  void ShutDown();   

  void SubmitCamera(Camera* _pCamera);
  void SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform);
  
  void Draw();

private:

  class Impl;
  std::unique_ptr<Impl> m_pImpl;    
};