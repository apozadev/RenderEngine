#include "Util/ModelLoader.h"

#include "Core/Exception.h"
#include "Core/ImageManager.h"
#include "File/FileUtils.h"
#include "Graphics/Material.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Vertex.h"
#include "Graphics/MaterialLibrary.h"
#include "Graphics/SamplerConfig.h"

//#include <fileapi.h>
//#include <algorithm>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "assimp/Importer.hpp"

void ProcessNode(aiNode* _pAssimpNode, const aiScene* _pAssimpScene, ModelComponent* pModelComp_);
void ProcessMaterials(const aiScene* _pAssimpScene, const Material* _pMaterial, std::string _sDirectory, ModelComponent* pModelComp_);
void ProcessMesh(aiMesh* _pAssimpMesh, const aiScene* _pAssimpScene, ModelComponent* pModelComp_);

void ModelLoader::LoadModel(const char* _sFilename, const Material* _pMaterial, ModelComponent* pModelComp_)
{

  Assimp::Importer oImporter;

  std::string sFullFilename(_sFilename);
  sFullFilename = file::GetWorkingDirectory() + sFullFilename;

  const aiScene* oScene = oImporter.ReadFile(sFullFilename.c_str(), aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_Triangulate);

  if (!oScene || oScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !oScene->mRootNode)
  {
    THROW_GENERIC_EXCEPTION(oImporter.GetErrorString())
      return;
  }
  
  std::string sDirectory = sFullFilename.substr(0, sFullFilename.find_last_of('/') + 1);  

  ProcessMaterials(oScene, _pMaterial, sDirectory, pModelComp_);

  ProcessNode(oScene->mRootNode, oScene, pModelComp_);
}

void ProcessNode(aiNode* _pAssimpNode, const aiScene* _pAssimpScene, ModelComponent* pModelComp_)
{

  for (unsigned int i = 0; i < _pAssimpNode->mNumMeshes; i++)
  {
    aiMesh* oAssimpMesh = _pAssimpScene->mMeshes[_pAssimpNode->mMeshes[i]];
    /*if (mesh->HasBones())
    {
      model->AddDrawable(processSkinnedMesh(mesh, scene, sceneGraph, sceneNode, model));
    }
    else
    {*/
    ProcessMesh(oAssimpMesh, _pAssimpScene, pModelComp_);
    //}
  }

  for (unsigned int i = 0; i < _pAssimpNode->mNumChildren; i++)
  {
    ProcessNode(_pAssimpNode->mChildren[i], _pAssimpScene, pModelComp_);
  }
}

void ProcessMaterials(const aiScene* _pAssimpScene, const Material* _pMaterial, std::string _sDirectory, ModelComponent* pModelComp_)
{  

  for (unsigned int i = 0; i < _pAssimpScene->mNumMaterials; i++)
  {
    aiMaterial* aiMat = _pAssimpScene->mMaterials[i];

    owner_ptr<MaterialInstance> pMatInstance = Factory::Create<MaterialInstance>();

    std::vector<aiTextureType> types = {
        aiTextureType_DIFFUSE, // t0
        aiTextureType_NORMALS, // t1
        aiTextureType_METALNESS, // t2
        //aiTextureType_DIFFUSE_ROUGHNESS, // t3
        aiTextureType_UNKNOWN // t3
    };

    SamplerConfig aSamplerConfigs[4];

    aSamplerConfigs[0].m_eAddressMode = TextureAddressMode::REPEAT;
    aSamplerConfigs[0].m_eMipmapFilterMode = TextureFilterMode::LINEAR;
    aSamplerConfigs[0].m_eMinFilterMode = TextureFilterMode::LINEAR;
    aSamplerConfigs[0].m_eMagFilterMode = TextureFilterMode::LINEAR;

    aSamplerConfigs[1] = aSamplerConfigs[0];
    aSamplerConfigs[1].m_eAddressMode = TextureAddressMode::REPEAT;

    aSamplerConfigs[2] = aSamplerConfigs[0];

    aSamplerConfigs[3] = aSamplerConfigs[0];

    for (int j = 0; j < types.size(); j++)
    {
      if (aiMat->GetTextureCount(types[j]) == 0) continue;
      aiString str;
      aiMat->Get(AI_MATKEY_TEXTURE(types[j], 0), str);
      
      if (const aiTexture* aiTex = _pAssimpScene->GetEmbeddedTexture(str.C_Str()))
      {
        //returned pointer is not null, read texture from memory
        if (aiTex->mHeight == 0)
        {
          // Compressed image data, we need to decode it
          const Image& oImage = ImageManager::GetInstance()->DecodeFromMemory(aiTex->mFilename.C_Str(), (unsigned char*)aiTex->pcData, aiTex->mWidth);
          if (oImage.m_pData)
          {
            owner_ptr<Texture2D> pTexture = Factory::Create<Texture2D>();
            pTexture->Configure(oImage, aSamplerConfigs[j]);
            pMatInstance->AddTexture(std::move(pTexture));
          }
        }
        else
        {
          Image oImage{};
          oImage.m_pData = aiTex->pcData;
          oImage.m_iWidth = aiTex->mWidth;
          oImage.m_iHeight = aiTex->mHeight;
          oImage.m_eFormat = ImageFormat::R8G8B8A8;

          owner_ptr<Texture2D> pTexture = Factory::Create<Texture2D>();
          pTexture->Configure(oImage, aSamplerConfigs[j]);
          pMatInstance->AddTexture(std::move(pTexture));          
        }
      }
      else //if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(str.C_Str()))
      {

        //regular file, read it from disk                
        owner_ptr<Texture2D> pTexture = Factory::Create<Texture2D>();
        pTexture->Configure(_sDirectory + str.C_Str(), aSamplerConfigs[j]);
        pMatInstance->AddTexture(std::move(pTexture));
      }
    }  
    pMatInstance->SetupSubState(_pMaterial);
    pModelComp_->AddMaterialInstance(std::move(pMatInstance));
  }
}

void ProcessMesh(aiMesh* _pAssimpMesh, const aiScene* _pAssimpScene, ModelComponent* pModelComp_)
{
  std::vector<Vertex> lstVertices(_pAssimpMesh->mNumVertices);
  std::vector<unsigned short> lstIndices(_pAssimpMesh->mNumFaces * 3u);

  aiVector3D minVertex = _pAssimpMesh->mVertices[0], maxVertex = _pAssimpMesh->mVertices[0];

  // vertices
  for (unsigned int i = 0; i < _pAssimpMesh->mNumVertices; i++)
  {
    Vertex oVertex;

    oVertex.m_vPos = {
        _pAssimpMesh->mVertices[i].x,
        _pAssimpMesh->mVertices[i].y,
        _pAssimpMesh->mVertices[i].z
    };

    if (_pAssimpMesh->HasNormals())
    {
      oVertex.m_vNormal = {
          _pAssimpMesh->mNormals[i].x,
          _pAssimpMesh->mNormals[i].y,
          _pAssimpMesh->mNormals[i].z
      };
    }

    if (_pAssimpMesh->mTextureCoords[0])
    {
      oVertex.m_vUv = {
          _pAssimpMesh->mTextureCoords[0][i].x,
          _pAssimpMesh->mTextureCoords[0][i].y
      };
    }
    else oVertex.m_vUv = { 0.0f, 0.0f };

    if (_pAssimpMesh->HasTangentsAndBitangents())
    {
      oVertex.m_vTangent = {
          _pAssimpMesh->mTangents[i].x,
          _pAssimpMesh->mTangents[i].y,
          _pAssimpMesh->mTangents[i].z
      };
    }    

    lstVertices[i] = oVertex;

    ////Get the smallest vertex 
    //minVertex.x = std::min(minVertex.x, oVertex.m_vPos.x);
    //minVertex.y = std::min(minVertex.y, oVertex.m_vPos.y);
    //minVertex.z = std::min(minVertex.z, oVertex.m_vPos.z);

    ////Get the largest vertex 
    //maxVertex.x = std::max(maxVertex.x, oVertex.m_vPos.x);
    //maxVertex.y = std::max(maxVertex.y, oVertex.m_vPos.y);
    //maxVertex.z = std::max(maxVertex.z, oVertex.m_vPos.z);
  }

  // indices
  for (unsigned int i = 0; i < _pAssimpMesh->mNumFaces; i++)
  {
    aiFace face = _pAssimpMesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++)
      lstIndices[i * 3u + j] = face.mIndices[j];
  }

  pModelComp_->AddMesh(lstVertices, lstIndices, _pAssimpMesh->mMaterialIndex);

}

void ModelLoader::SetupQuadModel(const Material* _pMaterial, ModelComponent* pModelComp_)
{

  std::vector<Vertex> lstVertices{
      {{-1.0f,  1.0f, 0.f}, {0, 0, -1}, {1, 0, 0}, {0, 0, 0}, {0, 0}},
      {{ 1.0f,  1.0f, 0.f}, {0, 0, -1}, {1, 0, 0}, {0, 0, 0}, {1, 0}},
      {{ 1.0f, -1.0f, 0.f}, {0, 0, -1}, {1, 0, 0}, {0, 0, 0}, {1, 1}},
      {{-1.0f, -1.0f, 0.f}, {0, 0, -1}, {1, 0, 0}, {0, 0, 0}, {0, 1}}
  };

  std::vector<unsigned short> lstIndices{
    3, 0, 1,
    2, 3, 1
  };

  owner_ptr<MaterialInstance> pMatInstance = Factory::Create<MaterialInstance>();

  pMatInstance->SetupSubState(_pMaterial);

  pModelComp_->AddMaterialInstance(std::move(pMatInstance));

  pModelComp_->AddMesh(lstVertices, lstIndices, 0u);
}

void ModelLoader::SetupCubeModel(const Material* _pMaterial, ModelComponent* pModelComp_)
{
  std::vector<Vertex> lstVertices{
    // + X
    {{ 1.0f,  1.0f , -1.0f }, {0, 0, 1}, {0, 1, 0}, {1, 0}}, // 0
    {{ 1.0f,  1.0f ,  1.0f }, {0, 0, 1}, {0, 1, 0}, {1, 1}},
    {{ 1.0f, -1.0f ,  1.0f }, {0, 0, 1}, {0, 1, 0}, {0, 1}},
    {{ 1.0f, -1.0f , -1.0f }, {0, 0, 1}, {0, 1, 0}, {0, 0}},

    // -X
    {{-1.0f,  1.0f , -1.0f }, {0, 0, -1}, {0, -1, 0}, {1, 0}}, // 4
    {{-1.0f,  1.0f ,  1.0f }, {0, 0, -1}, {0, -1, 0}, {1, 1}},
    {{-1.0f, -1.0f ,  1.0f }, {0, 0, -1}, {0, -1, 0}, {0, 1}},
    {{-1.0f, -1.0f , -1.0f }, {0, 0, -1}, {0, -1, 0}, {0, 0}},

    // +Y
    {{ 1.0f,  1.0f , -1.0f }, {0, 1, 0}, {0, 0, 1}, {1, 0}}, // 8
    {{ 1.0f,  1.0f ,  1.0f }, {0, 1, 0}, {0, 0, 1}, {1, 1}},
    {{-1.0f,  1.0f ,  1.0f }, {0, 1, 0}, {0, 0, 1}, {0, 1}},
    {{-1.0f,  1.0f , -1.0f }, {0, 1, 0}, {0, 0, 1}, {0, 0}},

    // -Y  
    {{ 1.0f,  -1.0f, -1.0f }, {0, -1, 0}, {0, 0, -1}, {1, 0}}, // 12
    {{ 1.0f,  -1.0f,  1.0f }, {0, -1, 0}, {0, 0, -1}, {1, 1}},
    {{-1.0f,  -1.0f,  1.0f }, {0, -1, 0}, {0, 0, -1}, {0, 1}},
    {{-1.0f,  -1.0f, -1.0f }, {0, -1, 0}, {0, 0, -1}, {0, 0}},

    // +Z  
    {{ 1.0f,  -1.0f,  1.0f }, {0, -1, 0}, {1, 0, 0}, {1, 0}}, // 16
    {{ 1.0f,   1.0f,  1.0f }, {0, -1, 0}, {1, 0, 0}, {1, 1}},
    {{-1.0f,   1.0f,  1.0f }, {0, -1, 0}, {1, 0, 0}, {0, 1}},
    {{-1.0f,  -1.0f,  1.0f }, {0, -1, 0}, {1, 0, 0}, {0, 0}},

    // -Z  
    {{ 1.0f,  -1.0f,  -1.0f}, {0, -1, 0}, {1, 0, 0}, {1, 0}}, // 20
    {{ 1.0f,   1.0f,  -1.0f}, {0, -1, 0}, {1, 0, 0}, {1, 1}},
    {{-1.0f,   1.0f,  -1.0f}, {0, -1, 0}, {1, 0, 0}, {0, 1}},
    {{-1.0f,  -1.0f,  -1.0f}, {0, -1, 0}, {1, 0, 0}, {0, 0}},
  };

  std::vector<unsigned short> lstIndices{
    0, 1, 2,      0, 2, 3,      // +X
    6, 5, 4,      7, 6, 4,      // -X
    10, 9, 8,     11, 10, 8,    // +Y
    12, 13, 14,   12, 14, 15,   // -Y
    16, 17, 18,   16, 18, 19,   // +Z
    22, 21, 20,   23, 22, 20,   // -Z
  };

  owner_ptr<MaterialInstance> pMatInstance = Factory::Create<MaterialInstance>();

  pMatInstance->SetupSubState(_pMaterial);

  pModelComp_->AddMaterialInstance(std::move(pMatInstance));

  pModelComp_->AddMesh(lstVertices, lstIndices, 0u);
}
