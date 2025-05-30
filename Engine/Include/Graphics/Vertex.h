#pragma once

#include "Graphics/PODTypes.h"

struct Vertex
{
  pod::Vector3 m_vPos;  
  pod::Vector3 m_vNormal;  
  pod::Vector3 m_vTangent;  
  pod::Vector3 m_vColor;
  pod::Vector2 m_vUv;
};