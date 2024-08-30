#include "Graphics/Material.h"
#include "Graphics/Resource.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Graphics/Window.h"
#include "Graphics/Mesh.h"
#include "Graphics/RenderStateInfo.h"
#include "Core/Exception.h"

class Material::Impl
{
public:
  api::APIRenderState* m_pAPIRenderState;
  std::vector<Resource*> m_lstResources;

  /*bool m_bBlendEnabled;
  BlendOp m_eBlendOp;
  BlendFactor m_eSrcBlendFactor;
  BlendFactor m_eDstBlendFactor; */

  Impl(Window* _pWindow
    , const std::string& _sVSFilename
    , const std::string& _sPSFilename
    , bool _bBlendEnabled
    , BlendOp _eBlendOp
    , BlendFactor _eSrcBlendFactor
    , BlendFactor _eDstBlendFactor
    , bool _bDepthWrite
    , bool _bDepthRead)
  {
    _pWindow->SetUsing();

    RenderStateInfo oInfo{};
    oInfo.m_uMeshConstantSize = sizeof(MeshConstant);
    oInfo.m_sVSFilename = _sVSFilename;
    oInfo.m_sPSFilename = _sPSFilename;    
    oInfo.m_bBlendEnabled = _bBlendEnabled;
    oInfo.m_eBlendOp = _eBlendOp;
    oInfo.m_eSrcBlendFactor = _eSrcBlendFactor;
    oInfo.m_eDstBlendFactor = _eDstBlendFactor;
    oInfo.m_bDepthWrite = _bDepthWrite;
    oInfo.m_bDepthRead = _bDepthRead;

    m_pAPIRenderState = api::CreateAPIRenderState(oInfo);
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
  m_pImpl = std::make_unique<Impl>(_pWindow, _sVSFilename, _sPSFilename, _bBlendEnabled, _eBlendOp, _eSrcBlendFactor, _eDstBlendFactor, _bDepthWrite, _bDepthRead);    
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
