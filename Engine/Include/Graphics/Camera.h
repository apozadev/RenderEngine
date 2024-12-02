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

class Camera
{

  friend class Renderer;

  struct GlobalBufferData
  {
    glm::mat4 m_mViewProj;
  };

public:

  Camera(const std::string& _sRenderPipelineId);
  Camera(Camera&& _oOther);
  ~Camera();  

  void UpdateTransform(const Transform& _oParentTransform);

  void Bind() const;  

  uint64_t GetKey() const;

  const std::string& GetRenderPipelineId() const;

  float m_fNear = 0.1f;
  float m_fFar = 100.f;

private:

  api::APICamera* m_pAPICamera;

  api::APIRenderSubState* m_pSubState;

  owner_ptr<ConstantBuffer<GlobalBufferData>> m_pCBuffer;

  std::string m_sRenderPipelineId;
};