#pragma once

#include <memory>
#include <string>

#include "glm/vec4.hpp"
#include "Core/Singleton.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/ResourceFrequency.h"
#include "Memory/PtrTypes.h"

class Mesh;
class MaterialInstance;
class Transform;
class Window;
class Camera;
struct DirLight;
struct RenderPipelineConfig;

template<typename T> class ConstantBuffer;

#define MAX_LIGHTS 5

class Renderer : public Singleton<Renderer>
{  

  struct DirLightData
  {
    glm::vec4 m_vDir;    
    glm::vec4 m_vColor;    
  };
  
  struct alignas(16) LightData
  {
    DirLightData m_aLights[MAX_LIGHTS];
    unsigned int m_uNumLights;
    float pad0, pad1, pad2;
  };

public:  

  struct CamView
  {
    Camera* m_pCamera;
    const Transform* m_pTransform;
  };

  Renderer();
  ~Renderer();

  void Initialize();
  void InitializePostWindow();
  void ShutDownPreWindow();   
  void ShutDownPostWindow();   

  void AddRenderPipeline(RenderPipelineConfig&& _pPipelineCofig);  

  void Setup();

  void SubmitCamera(Camera* _pCamera, const Transform* _pTransform);
  void SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform);
  void SubmitDirLight(DirLight* _pDirLight, const Transform* _pTransform);

  void OnWindowResize();

  RenderPipeline* GetRenderPipeline(std::string _sPipelineId);

  void SetupSubStateLightCBuffers(ResourceFrequency _eFrequency);
  
  void Draw();  

private:  

  std::vector<CamView> m_lstCamViews;  

  std::vector<RenderPipeline> m_lstRenderPipelines;  

  owner_ptr<ConstantBuffer<LightData>> m_pLightCBuff;
};