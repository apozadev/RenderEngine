#pragma once

#include <memory>
#include <string>

#include "Core/Singleton.h"
#include "Graphics/RenderPipeline.h"

class Mesh;
class MaterialInstance;
class Transform;
class Window;
class Camera;
struct RenderPipelineConfig;

class Renderer : public Singleton<Renderer>
{

  struct CamView
  {
    Camera* m_pCamera;
    const Transform* m_pTransform;
  };

public:  

  Renderer();
  ~Renderer();

  void Initialize();
  void ShutDownPreWindow();   
  void ShutDownPostWindow();   

  void AddRenderPipeline(RenderPipelineConfig&& _pPipelineCofig);  

  void SubmitCamera(Camera* _pCamera, const Transform* _pTransform);
  void SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform);

  void OnWindowResize();

  RenderPipeline* GetRenderPipeline(std::string _sPipelineId);
  
  void Draw();

private:  

  std::vector<CamView> m_lstCamViews;

  std::vector<RenderPipeline> m_lstRenderPipelines;
};