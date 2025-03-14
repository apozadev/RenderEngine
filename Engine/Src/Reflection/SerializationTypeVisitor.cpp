#include "Reflection/SerializationTypeVisitor.h"
#include "Reflection/ReflectionHelper.h"
#include "../3rd/rapidxml/rapidxml.hpp"

namespace reflection
{

  void SerializationTypeVisitor::Visit(const TypeDescriptor* type)
  {
    if (!m_varName)
    {
      std::string* name = new std::string(type->getFullName());
      ReflectionHelper::TrackString(name);
      m_varName = name->c_str();
    }
    rapidxml::xml_node<>* newNode = m_doc->allocate_node(rapidxml::node_type::node_element, m_varName);
    m_xmlParent->append_node(newNode);

    std::string* str = new std::string(type->GetValueStr(m_pObj));
    ReflectionHelper::TrackString(str);

    newNode->value(str->c_str());
  }

  void SerializationTypeVisitor::Visit(const TypeDescriptor_Struct* type)
  {
    if (!m_varName)
    {
      m_varName = type->name;
    }

    rapidxml::xml_node<>* newNode = m_doc->allocate_node(rapidxml::node_type::node_element, m_varName);
    m_xmlParent->append_node(newNode);
    m_xmlParent = newNode;

    SerializeMembers(type);
  }

  void SerializationTypeVisitor::Visit(const TypeDescriptor_StdVector* type)
  {
    if (!m_varName)
    {
      std::string* name = new std::string(type->getFullName());
      ReflectionHelper::TrackString(name);
      m_varName = name->c_str();
    }

    rapidxml::xml_node<>* newNode = m_doc->allocate_node(rapidxml::node_type::node_element, m_varName);
    m_xmlParent->append_node(newNode);

    size_t size = type->getSize(m_pObj);
    const void* pObj = m_pObj;

    for (int i = 0; i < size; i++)
    {
      m_pObj = type->getItem(pObj, i);
      m_varName = nullptr;
      m_xmlParent = newNode;
      type->itemType->Accept(this);
    }
  }

  void SerializationTypeVisitor::Visit(const TypeDescriptor_Weak_Ptr* type)
  {
    const void** ppObj = (const void**)m_pObj;

    if (!m_varName)
    {
      std::string* name = new std::string(type->getStaticType()->getFullName() + "*");
      ReflectionHelper::TrackString(name);
      m_varName = name->c_str();
    }

    rapidxml::xml_node<>* newNode = m_doc->allocate_node(rapidxml::node_type::node_element, m_varName);
    m_xmlParent->append_node(newNode);

    std::string* str = new std::string(std::to_string((unsigned int)*ppObj));
    ReflectionHelper::TrackString(str);

    newNode->value(str->c_str());
  }

  void SerializationTypeVisitor::Visit(const TypeDescriptor_Owner_Ptr* type)
  {
    const owner_ptr<void>* ppObj = (owner_ptr<void>*)m_pObj;

    const TypeDescriptor* concreteType = type->getDynamicType(ppObj->get());
    const rapidxml::xml_node<>* xmlParent = m_xmlParent;

    if (!m_varName)
    {
      std::string* name = new std::string(concreteType->getFullName() + "_Ptr");
      ReflectionHelper::TrackString(name);
      m_varName = name->c_str();
    }

    rapidxml::xml_node<>* firstChild = nullptr;
    if (xmlParent->first_node())
    {
      firstChild = xmlParent->last_node();
    }

    m_pObj = ppObj->get();
    concreteType->Accept(this);

    rapidxml::xml_node<>* newNode = firstChild ? firstChild->next_sibling() : xmlParent->first_node();

    if (!newNode)
    {
      return;
    }

    std::string* str = new std::string(std::to_string((unsigned int)ppObj->get()));
    ReflectionHelper::TrackString(str);
    rapidxml::xml_attribute<>* idAtt = m_doc->allocate_attribute(__PtrIdAttName, str->c_str());
    newNode->append_attribute(idAtt);

    std::string* typeName = new std::string(concreteType->getFullName());
    ReflectionHelper::TrackString(typeName);
    rapidxml::xml_attribute<>* typeAtt = m_doc->allocate_attribute(__PtrTypeAttName, typeName->c_str());
    newNode->append_attribute(typeAtt);
  }

  void SerializationTypeVisitor::SerializeMembers(const TypeDescriptor_Struct* type)
  {
    const void* pObj = m_pObj;

    rapidxml::xml_node<>* xmlParent = m_xmlParent;

    if (type->parentTypeDesc)
    {
      SerializeMembers(type->parentTypeDesc);
    }

    for (const TypeDescriptor_Struct::Member& member : type->members)
    {
      m_pObj = member.getAddress(pObj);
      m_varName = member.name;
      m_xmlParent = xmlParent;
      member.type->Accept(this);
    }
  }

}