#pragma once

#include "Reflection/UnloadTypeVisitor.h"

namespace reflection
{

  void UnloadTypeVisitor::Visit(const TypeDescriptor_Struct* type)
  {
    UnloadMembers(type);
  }

  void UnloadTypeVisitor::UnloadMembers(const TypeDescriptor_Struct* type)
  {
    void* pObj = m_pObj;

    if (type->parentTypeDesc)
    {
      UnloadMembers(type->parentTypeDesc);
    }

    int memberCount = static_cast<int>(type->members.size());
    for (int i = 0; i < memberCount; i++)
    {
      m_pObj = type->members[i].getAddress(pObj);
      type->members[i].type->Accept(this);
    }
  }

  void UnloadTypeVisitor::Visit(const TypeDescriptor_StdVector* type)
  {

    void* pObj = m_pObj;

    size_t size = type->getSize(pObj);

    for (size_t i = 0; i < size; i++)
    {
      m_pObj = type->getItem(pObj, i);

      type->itemType->Accept(this);
    }
  }

  void UnloadTypeVisitor::Visit(const TypeDescriptor_Owner_Ptr* type)
  {

    owner_ptr<void>* ppObj = (owner_ptr<void>*)m_pObj;

    const TypeDescriptor* dynamicType = type->getDynamicType(ppObj->get());

    m_pObj = ppObj->get();

    dynamicType->Accept(this);

  }

}