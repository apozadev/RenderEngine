#pragma once

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>

#include "Graphics/ConstantBuffer.h"
#include "Graphics/API/GraphicsAPI.h"

#include "Memory/PtrTypes.h"

class Window;
class Transform;
class RenderPipeline;
class Texture2D;

class Camera
{

  friend class Renderer;

public:

  struct GlobalBufferData
  {
    glm::mat4 m_mViewProj;
    glm::mat4 m_mView;
    glm::mat4 m_mViewInv;
    glm::mat4 m_mProj;
  };
  
  //Camera(Camera&& _oOther);
  ~Camera();  

  void Configure(const std::string& _sRenderPipelineId, bool _bOrtho = false, float _fAspect = -1.f);

  void UpdateTransform(const Transform& _oParentTransform);  

  uint64_t GetKey() const;

  const std::string& GetRenderPipelineId() const { return m_sRenderPipelineId; }

  glm::mat4x4 GetProjMatrix();

public:

  float m_fNear = 0.1f;
  float m_fFar = 100.f;  

  api::APIRenderSubState* m_pSubState;

  owner_ptr<ConstantBuffer<GlobalBufferData>> m_pCBuffer;

  std::string m_sRenderPipelineId;

  bool m_bOrtho;

  float m_fAspect = -1.f;
};