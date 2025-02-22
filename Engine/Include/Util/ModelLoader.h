#pragma once

#include "Core/Singleton.h"

#include "Components/ModelComponent.h"

#include <vector>

class Material;

class ModelLoader : public Singleton<ModelLoader>
{
public:

  void LoadModel(const char* _sFilename, const Material* _pMaterial, ModelComponent* pModelComp_);

  void SetupQuadModel(const Material* _pMaterial, ModelComponent* pModelComp_);

};