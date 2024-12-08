#include "Components/DirLightComponent.h"
#include "Core/Entity.h"
#include "Graphics/Renderer.h"
#include "Memory/Factory.h"

DirLightComponent::DirLightComponent()
{
  m_oDirLight.m_vColor = { 1.f, 1.f, 1.f };
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
  Renderer::GetInstance()->SubmitDirLight(&m_oDirLight, &m_pEntity->GetGlobalTransform());
}
