#pragma once

#include "Reflection/CopyTypeVisitor.h"

namespace reflection
{

  void CopyTypeVisitor::Visit(const reflection::TypeDescriptor* type)
  {
    if (strcmp(type->name, "std::string") == 0)
    {
      std::string* strDst = (std::string*)m_pObjDst;
      std::string* strSrc = (std::string*)m_pObjSrc;
      *strDst = *strSrc;
    }
    else
    {
      memcpy(m_pObjDst, m_pObjSrc, type->size);
    }
  }

  void CopyTypeVisitor::Visit(const TypeDescriptor_Struct* type)
  {
    const void* pObjSrc = m_pObjSrc;
    void* pObjDst = m_pObjDst;

    size_t memberCount = type->members.size();
    for (size_t i = 0; i < memberCount; i++)
    {
      m_pObjDst = type->members[i].getAddress(pObjDst);
      m_pObjSrc = type->members[i].getAddress(pObjSrc);
      type->members[i].type->Accept(this);
    }
  }

  void CopyTypeVisitor::Visit(const TypeDescriptor_StdVector* type)
  {
    owner_ptr<void>* ppObjSrc = (owner_ptr<void>*)m_pObjSrc;    
    owner_ptr<void>* ppObjDst = (owner_ptr<void>*)m_pObjDst;    

    size_t srcSize = type->getSize(ppObjSrc->get());
    size_t dstSize = srcSize;

    // We must check for ignored items
    /*for (size_t i = 0; i < srcSize; i++) {
      void* pItem = type->getItem(pObjSrc, i);
      if (type->itemType->getDynamic(pItem)->create == nullptr) {
        dstSize--;
      }
    }*/

    type->resize(ppObjDst->get(), dstSize);

    for (size_t i = 0; i < srcSize; i++) {
      m_pObjSrc = type->getItem(ppObjSrc->get(), i);
      m_pObjDst = type->getItem(ppObjDst->get(), i);
      type->itemType->Accept(this);
    }
  }

  void CopyTypeVisitor::Visit(const TypeDescriptor_Owner_Ptr* type)
  {
      owner_ptr<void>* ppObjSrc = (owner_ptr<void>*)m_pObjSrc;
      owner_ptr<void>* ppObjDst = (owner_ptr<void>*)m_pObjDst;

      const TypeDescriptor* dynamicTypeSrc = type->getDynamicType(ppObjSrc);

      // TypeDescriptor_Ignored does not have these function pointers assigned
      if (dynamicTypeSrc->create == nullptr) {
        return;
      }

      if (ppObjDst->get() != nullptr) {
        const TypeDescriptor* dynamicTypeDst = type->getDynamicType(ppObjDst->get());
        // TODO: This is a memory leak, think of something
        ppObjDst->reset();
      }

      *ppObjDst = dynamicTypeSrc->create();

      m_pObjSrc = ppObjSrc->get();
      m_pObjDst = ppObjDst->get();
      dynamicTypeSrc->Accept(this);
  }

  void CopyTypeVisitor::Visit(const reflection::TypeDescriptor_Asset_Ptr* type) {
    /*const void** ppObjSrc = type->getPPtrConst(m_pObjSrc);
    void** ppObjDst = type->getPPtr(m_pObjDst);
    const TypeDescriptor* dynamicType = type->GetDynamic(ppObjSrc);         
    type->setFilename(m_pObjDst, type->getFilename(m_pObjSrc));
    *type->getPPtr(m_pObjDst) = *type->getPPtr(m_pObjSrc);*/
    //*ppObjDst = PrefabManager::GetInstance()->LoadPrefab(filename.c_str(), dynamicType);    
  }

  }

