#pragma once

#include <memory>
#include <vector>

#include "Graphics/Window.h"
#include "Graphics/Material.h"
#include "Graphics/API/GraphicsAPI.h"
#include "Memory/PtrTypes.h"
#include "Core/BaseObject.h"

class Resource;

class MaterialInstance : public BaseObject
{
public: 

  void AddTexture(owner_ptr<Texture2D>&& _pTexture)
  {
    m_lstTextures.push_back(std::move(_pTexture));
  }

  Material* GetMaterial() const { return m_pMaterial; }

  uint16_t GetId() const { return m_uId; }

  void Setup(Material* _pMaterial);

  void Bind() const;    

private:  

  std::vector<owner_ptr<Texture2D>> m_lstTextures;
  std::vector<ConstantBufferBase*> m_lstCBuffers;

  bool m_bSetup;

  uint16_t m_uId;

  Material* m_pMaterial;

  api::APIRenderSubState* m_pSubState;

};