#include "Components/DirLightComponent.h"
#include "Core/Entity.h"
#include "Graphics/Renderer.h"
#include "Memory/Factory.h"

DirLightComponent::DirLightComponent()
{
}

DirLightComponent::~DirLightComponent() 
{
}

void DirLightComponent::Start()
{
  m_pDirLight = Factory::Create<DirLight>();
}

void DirLightComponent::PreTransformUpdate(float _fTimeStep)
{
}

void DirLightComponent::Update(float _fTimeStep)
{
  Renderer::GetInstance()->SubmitDirLight(m_pDirLight.get(), &m_pEntity->GetGlobalTransform());
}
