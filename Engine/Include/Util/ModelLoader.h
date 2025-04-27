#pragma once

#include "Core/Singleton.h"

#include "Components/ModelComponent.h"

#include <vector>

class Material;

void LoadModel(const char* _sFilename, const Material* _pMaterial, ModelComponent* pModelComp_);

void SetupQuadModel(const Material* _pMaterial, ModelComponent* pModelComp_);

void SetupCubeModel(const Material* _pMaterial, ModelComponent* pModelComp_);

void SetupQuadMesh(Mesh* pMesh_);

void SetupCubeMesh(Mesh* pMesh_);
