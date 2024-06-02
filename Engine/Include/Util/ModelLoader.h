#pragma once

#include "Core/Singleton.h"

#include "Components/ModelComponent.h"

#include <vector>

class ModelLoader : public Singleton<ModelLoader>
{
public:

  void LoadModel(const char* _sFilename, Window* _pWindow, ModelComponent* pModelComp_);

};