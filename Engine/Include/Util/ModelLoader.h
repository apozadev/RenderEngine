#pragma once

#include "Core/Singleton.h"

#include "Components/ModelComponent.h"

#include <vector>

class Material;

class ModelLoader : public Singleton<ModelLoader>
{
public:

  void LoadModel(const char* _sFilename, Material* _pMaterial, Window* _pWindow, ModelComponent* pModelComp_);

};