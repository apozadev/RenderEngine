#pragma once

#include "Reflection/TypeVisitor.h"

namespace rapidxml {
  template<class Ch = char>
  class xml_node;
}

namespace reflection {
  class DeserializationTypeVisitor : public TypeVisitor {
  public:
    DeserializationTypeVisitor(void* pObj, const rapidxml::xml_node<>* xmlNode) :
      m_pObj(pObj),
      m_xmlNode(xmlNode)
    {}
    void Visit(const TypeDescriptor* type) override;
    void Visit(const TypeDescriptor_Struct* type) override;
    void Visit(const TypeDescriptor_StdVector* type) override;
    void Visit(const TypeDescriptor_Weak_Ptr* type) override;
    void Visit(const TypeDescriptor_Owner_Ptr* type) override;
    void Visit(const TypeDescriptor_Asset_Ptr* type) override;
  private:

    void DeserializeMembers(const TypeDescriptor_Struct* type);

    void* m_pObj;
    const rapidxml::xml_node<>* m_xmlNode;
  };

  class ConfigureTypeVisitor : public TypeVisitor {
  public:
    ConfigureTypeVisitor(void* pObj) :
      m_pObj(pObj)
    {}
    void Visit(const TypeDescriptor* type) override {}
    void Visit(const TypeDescriptor_Struct* type) override;
    void Visit(const TypeDescriptor_StdVector* type) override;
    void Visit(const TypeDescriptor_Weak_Ptr* type) override {}
    void Visit(const TypeDescriptor_Owner_Ptr* type) override;
    void Visit(const TypeDescriptor_Asset_Ptr* type) override {};
  private:
    void SetupMembers(const TypeDescriptor_Struct* type);
    void* m_pObj;
  };
}