#pragma once

#include <memory>
#include <string>

#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Core/Singleton.h"
#include "Graphics/RenderPipeline.h"
#include "Graphics/ResourceFrequency.h"
#include "Memory/PtrTypes.h"

class Mesh;
class MaterialInstance;
class Transform;
class Window;
class Camera;
class Texture2D;
class RenderTarget;
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
    glm::mat4 m_aLightViews[MAX_LIGHTS];
    unsigned int m_uNumLights;
    unsigned int m_uNumShadows;
    float pad0, pad1;
  };

public:  

  struct CamView
  {
    Camera* m_pCamera;
    const Transform* m_pTransform;
  };

  struct ShadowView
  {
    Camera* m_pCamera;
    const Transform* m_pTransform;
    const RenderTarget* m_pShadowMap;
    const Pass* m_pPass;
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
  void SubmitMesh(Mesh* _pMesh, const MaterialInstance* _pMaterial, const Transform* _pTransform, bool _bCastShadow);
  void SubmitDirLight(const glm::vec3& _vColor, Camera* _pCamera, const Transform* _pTransform, const RenderTarget* _pShadowMap, const Pass* _pPass);

  void OnWindowResize();

  RenderPipeline* GetRenderPipeline(std::string _sPipelineId);

  void SetupSubStateLightCBuffers(ResourceFrequency _eFrequency);

  void SetupSubStateShadowMaps(ResourceFrequency _eFrequency);  
  
  void Draw();  

private:  

  std::vector<CamView> m_lstCamViews;  

  std::vector<RenderPipeline> m_lstRenderPipelines;  

  owner_ptr<ConstantBuffer<LightData>> m_pLightCBuff;  

  std::vector<ShadowView> m_lstShadowViews;  

  std::vector<Job> m_lstShadowJobs;    

  owner_ptr<RenderTarget> m_pEnvMapDiff;
  owner_ptr<RenderTarget> m_pEnvMapSpec;
};