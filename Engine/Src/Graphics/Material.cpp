#include "Graphics/Material.h"
#include "Graphics/Resource.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Window.h"
#include "Graphics/Mesh.h"
#include "Core/Exception.h"

class Material::Impl
{
public:
  api::APIRenderState* m_pAPIRenderState;
  std::vector<Resource*> m_lstResources;  

  Impl(Window* _pWindow, const RenderStateInfo& _rInfo)
  {
    _pWindow->SetUsing();

    m_pAPIRenderState = api::CreateAPIRenderState(_rInfo);

    // 63       | 55            | 54        | 22
    // -------------------------------------------------
    // 8 window | 1 translucent | 32 depth

    // m_uRenderKey = static_cast<uint64_t>(_bBlendEnabled ? 1 : 0) << (55);    
  }

  ~Impl()
  {
    for (Resource* pResource : m_lstResources)
    {
      delete pResource;
    }

    api::DestroyAPIRenderState(m_pAPIRenderState);
  }
};

Material::Material(Window* _pWindow
  , const std::string& _sVSFilename
  , const std::string& _sPSFilename
  , bool _bBlendEnabled
  , BlendOp _eBlendOp
  , BlendFactor _eSrcBlendFactor
  , BlendFactor _eDstBlendFactor
  , bool _bDepthWrite
  , bool _bDepthRead)
{

  m_oInfo = {};
  m_oInfo.m_uMeshConstantSize = sizeof(MeshConstant);
  m_oInfo.m_sVSFilename = _sVSFilename;
  m_oInfo.m_sPSFilename = _sPSFilename;
  m_oInfo.m_bBlendEnabled = _bBlendEnabled;
  m_oInfo.m_eBlendOp = _eBlendOp;
  m_oInfo.m_eSrcBlendFactor = _eSrcBlendFactor;
  m_oInfo.m_eDstBlendFactor = _eDstBlendFactor;
  m_oInfo.m_bDepthWrite = _bDepthWrite;
  m_oInfo.m_bDepthRead = _bDepthRead;
  
  m_pImpl = std::make_unique<Impl>(_pWindow, m_oInfo);    
}

Material::Material(Material&& rMaterial)
  : m_pImpl(std::move(rMaterial.m_pImpl))
{
}

Material::~Material()
{
}

void Material::Setup() const
{
  api::BeginRenderStateSetup(m_pImpl->m_pAPIRenderState);

  for (Resource* pResource : m_pImpl->m_lstResources)
  {
    pResource->Setup(ResourceFrequency::MATERIAL);
  }

  api::EndRenderStateSetup();
}

void Material::Bind() const
{

  api::BindAPIRenderState(m_pImpl->m_pAPIRenderState);

  for (const Resource* pResource : m_pImpl->m_lstResources)
  {
    pResource->Bind();
  }
}

void Material::SetUsing() const
{
  api::SetUsingAPIRenderState(m_pImpl->m_pAPIRenderState);
}

void Material::AddResourceInternal(Resource* _pResource)
{
  m_pImpl->m_lstResources.push_back(_pResource);
}

Material& Material::operator=(Material&& _rMaterial)
{  
  m_pImpl = std::move(_rMaterial.m_pImpl);
  return *this;
}
