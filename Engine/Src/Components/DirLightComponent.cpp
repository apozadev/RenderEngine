#include "Components/DirLightComponent.h"
#include "Core/Entity.h"
#include "Graphics/Renderer.h"
#include "Memory/Factory.h"
#include "Reflection/ReflectionImplMacros.h"

DirLightComponent::DirLightComponent()
{
  m_pShadowMap = Factory::Create<RenderTarget>();
  m_pShadowMap->Configure(0u, 1024, 1024, ImageFormat::R32, true);

  m_pShadowMap->SetUsing();

  m_pShadowPass = Factory::Create<Pass>();
  m_pShadowPass->Configure("Assets/Shaders/Vertex/VertexShader.hlsl", "Assets/Shaders/Pixel/SolidPixel.hlsl"
    , false
    , BlendOp::BLEND_OP_MIN
    , BlendFactor::BLEND_DEST_ALPHA
    , BlendFactor::BLEND_ZERO
    , true, true    
    , m_pShadowMap.get()
    , 0);
  m_pShadowPass->Setup();

  float fAspect = (float)m_pShadowMap->GetDepthStencilTexture()->GetWidth() / (float)m_pShadowMap->GetDepthStencilTexture()->GetHeight();

  m_pCamera = Factory::Create<Camera>();
  m_pCamera->Configure("", true, fAspect);
}

DirLightComponent::~DirLightComponent() 
{
}

void DirLightComponent::Start()
{  
}

void DirLightComponent::PreTransformUpdate(float _fTimeStep)
{
}

void DirLightComponent::Update(float _fTimeStep)
{
  const DirLightComponentDesc* pLightCompDesc = dynamic_cast<const DirLightComponentDesc*>(m_pComponentDesc.get());
  const Vec3& vColor = pLightCompDesc->m_vColor;

  m_pCamera->UpdateTransform(m_pEntity->GetGlobalTransform());
  Renderer::GetInstance()->SubmitDirLight(glm::vec3{vColor.x, vColor.y, vColor.z}, m_pCamera.get(), &m_pEntity->GetGlobalTransform(), m_pShadowMap.get(), m_pShadowPass.get());
}

REFLECT_STRUCT_BEGIN(DirLightComponent, Component)
REFLECT_STRUCT_END(DirLightComponent)

IMPLEMENT_COMPONENT_DESC_BEGIN(DirLightComponentDesc)
REFLECT_STRUCT_MEMBER(m_vColor)
IMPLEMENT_COMPONENT_DESC_END(DirLightComponentDesc)
