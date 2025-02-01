#include "Components/DirLightComponent.h"
#include "Core/Entity.h"
#include "Graphics/Renderer.h"
#include "Memory/Factory.h"

DirLightComponent::DirLightComponent()
{
  m_oDirLight.m_vColor = { 1.f, 1.f, 1.f };

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
  m_pCamera->UpdateTransform(m_pEntity->GetGlobalTransform());
  Renderer::GetInstance()->SubmitDirLight(&m_oDirLight, m_pCamera.get(), &m_pEntity->GetGlobalTransform(), m_pShadowMap.get(), m_pShadowPass.get());
}
