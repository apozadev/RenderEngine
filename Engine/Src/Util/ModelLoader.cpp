#include "Util/ModelLoader.h"

#include "Core/Exception.h"
#include "Core/ImageManager.h"
#include "File/FileUtils.h"
#include "Graphics/Material.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Vertex.h"
#include "Graphics/MaterialLibrary.h"

//#include <fileapi.h>
//#include <algorithm>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "assimp/Importer.hpp"

void ProcessNode(aiNode* _pAssimpNode, const aiScene* _pAssimpScene, Window* _pWindow, ModelComponent* pModelComp_);
void ProcessMaterials(const aiScene* _pAssimpScene, Material* _pMaterial, std::string _sDirectory, ModelComponent* pModelComp_);
void ProcessMesh(aiMesh* _pAssimpMesh, const aiScene* _pAssimpScene, Window* _pWindow, ModelComponent* pModelComp_);

void ModelLoader::LoadModel(const char* _sFilename, Window* _pWindow, ModelComponent* pModelComp_)
{
  Assimp::Importer oImporter;

  std::string sFullFilename(_sFilename);
  sFullFilename = file::GetWorkingDirectory() + sFullFilename;

  const aiScene* oScene = oImporter.ReadFile(sFullFilename.c_str(), aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace /* | aiProcess_PreTransformVertices */);

  if (!oScene || oScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !oScene->mRootNode)
  {
    THROW_GENERIC_EXCEPTION(oImporter.GetErrorString())
      return;
  }
  
  std::string sDirectory = sFullFilename.substr(0, sFullFilename.find_last_of('/') + 1);

  Material* pMaterial = MaterialLibrary::GetInstance()->CreateMaterial(_pWindow);

  pMaterial->Setup();

  ProcessMaterials(oScene, pMaterial, sDirectory, pModelComp_);

  ProcessNode(oScene->mRootNode, oScene, _pWindow, pModelComp_);

}

void ProcessNode(aiNode* _pAssimpNode, const aiScene* _pAssimpScene, Window* _pWindow, ModelComponent* pModelComp_)
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
    ProcessMesh(oAssimpMesh, _pAssimpScene, _pWindow, pModelComp_);
    //}
  }

  for (unsigned int i = 0; i < _pAssimpNode->mNumChildren; i++)
  {
    ProcessNode(_pAssimpNode->mChildren[i], _pAssimpScene, _pWindow, pModelComp_);
  }
}

void ProcessMaterials(const aiScene* _pAssimpScene, Material* _pMaterial, std::string _sDirectory, ModelComponent* pModelComp_)
{
  for (unsigned int i = 0; i < _pAssimpScene->mNumMaterials; i++)
  {
    aiMaterial* aiMat = _pAssimpScene->mMaterials[i];
    MaterialInstance& rMatInstance = pModelComp_->AddMaterial(MaterialInstance(_pMaterial));
    std::vector<aiTextureType> types = {
        aiTextureType_DIFFUSE, // t0
        aiTextureType_NORMALS, // t1
        aiTextureType_METALNESS, // t2
        //aiTextureType_DIFFUSE_ROUGHNESS, // t3
        aiTextureType_UNKNOWN // t3
    };
    for (int j = 0; j < types.size(); j++)
    {
      if (aiMat->GetTextureCount(types[j]) == 0) continue;
      aiString str;
      aiMat->Get(AI_MATKEY_TEXTURE(types[j], 0), str);
      /*
      if (const aiTexture* aiTex = _pAssimpScene->GetEmbeddedTexture(str.C_Str()))
      {
        //returned pointer is not null, read texture from memory

        if (aiTex->mHeight == 0)
        {
          // Compressed image data, we need to decode it
          Image img = ImageManager::decodeFromMemory(aiTex->mFilename.C_Str(), (unsigned char*)aiTex->pcData, aiTex->mWidth);
          if (img.data)
          {
            gfx::Texture2D* texture = memory::Factory::Create < gfx::Texture2D>((unsigned char*)img.data, img.width, img.height, img.channels, SRV_FREE_SLOT + j);
            matInstance->AddBindable(texture);
          }
        }
        else
        {
          gfx::Texture2D* texture = memory::Factory::Create < gfx::Texture2D>((unsigned char*)aiTex->pcData, aiTex->mWidth, aiTex->mHeight, 4, SRV_FREE_SLOT + j);
          matInstance->AddBindable(texture);
        }
      }
      else
        */
      //if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(str.C_Str()))
      {

        //regular file, read it from disk        
        rMatInstance.AddResource<Texture2D>(_sDirectory + str.C_Str(), j, PipelineStage::PIXEL);
      }
    }   

    rMatInstance.Setup();
  }
}

void ProcessMesh(aiMesh* _pAssimpMesh, const aiScene* _pAssimpScene, Window* _pWindow, ModelComponent* pModelComp_)
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

    /*if (mesh->HasNormals())
    {
      oVertex.m_vNormal = {
          _pMesh->mNormals[i].x,
          _pMesh->mNormals[i].y,
          _pMesh->mNormals[i].z
      };
    }*/

    if (_pAssimpMesh->mTextureCoords[0])
    {
      oVertex.m_vUv = {
          _pAssimpMesh->mTextureCoords[0][i].x,
          _pAssimpMesh->mTextureCoords[0][i].y
      };
    }
    else oVertex.m_vUv = { 0.0f, 0.0f };

    /*if (mesh->HasTangentsAndBitangents())
    {
      oVertex.tangent = {
          _pMesh->mTangents[i].x,
          _pMesh->mTangents[i].y,
          _pMesh->mTangents[i].z
      };
    }
    else oVertex.uv = { 0.0f, 0.0f };*/

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

  pModelComp_->AddMesh(lstVertices, lstIndices, _pAssimpMesh->mMaterialIndex, _pWindow);

}