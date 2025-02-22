#pragma once

#include "Reflection/TypeVisitor.h"

namespace reflection {

  class CopyTypeVisitor : public reflection::TypeVisitor {
  public:
    CopyTypeVisitor(void* pObjDst, const void* pObjSrc) : m_pObjDst(pObjDst), m_pObjSrc(pObjSrc) {}
    void Visit(const reflection::TypeDescriptor* type);
    void Visit(const reflection::TypeDescriptor_Struct* type);
    void Visit(const reflection::TypeDescriptor_StdVector* type);
    void Visit(const reflection::TypeDescriptor_Weak_Ptr* type) { this->Visit((reflection::TypeDescriptor*)type); }
    void Visit(const reflection::TypeDescriptor_Owner_Ptr* type);
    void Visit(const reflection::TypeDescriptor_Asset_Ptr* type);
  private:
    void* m_pObjDst;
    const void* m_pObjSrc;
  };
}
