#include "Reflection/DeserializationTypeVisitor.h"
#include "Reflection/ReflectionHelper.h"
#include "Memory/Factory.h"
#include "../3rd/rapidxml/rapidxml.hpp"

namespace reflection
{

  void DeserializationTypeVisitor::Visit(const TypeDescriptor* type)
  {
    type->SetValueFromString(m_pObj, m_xmlNode->value());
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Struct* type)
  {
    DeserializeMembers(type);
  }

  void DeserializationTypeVisitor::DeserializeMembers(const TypeDescriptor_Struct* type)
  {
    void* pObj = m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;
    if (type->parentTypeDesc)
    {
      DeserializeMembers(type->parentTypeDesc);
    }

    rapidxml::xml_node<>* child = xmlNode->first_node();

    while (child)
    {
      for (const TypeDescriptor_Struct::Member& member : type->members)
      {
        if (strcmp(member.name, child->name()) == 0)
        {
          m_pObj = member.getAddress(pObj);
          m_xmlNode = child;
          member.type->Accept(this);
          break;
        }
      }
      child = child->next_sibling();
    }
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_StdVector* type)
  {
    void* pObj = m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;

    size_t count = 0;
    rapidxml::xml_node<>* child = xmlNode->first_node();
    while (child)
    {
      count++;
      child = child->next_sibling();
    }
    type->resize(pObj, count);
    child = xmlNode->first_node();
    count = 0;
    while (child)
    {
      m_pObj = type->getItem(pObj, count);
      m_xmlNode = child;
      type->itemType->Accept(this);
      child = child->next_sibling();
      count++;
    }
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Weak_Ptr* type)
  {
    intptr_t id = std::atoll(m_xmlNode->value());
    ReflectionHelper::RegisterPendingPtr((void**)m_pObj, id);
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Owner_Ptr* type)
  {
    owner_ptr<void>* ppObj = (owner_ptr<void>*)m_pObj;
    const rapidxml::xml_node<>* xmlNode = m_xmlNode;

    std::string name;
    intptr_t ptrId = std::numeric_limits<intptr_t>::max();

    rapidxml::xml_attribute<>* att = xmlNode->first_attribute();

    while (att)
    {
      if (strcmp(att->name(), __PtrIdAttName) == 0)
      {
        ptrId = std::atoll(att->value());
      }
      else if (strcmp(att->name(), __PtrTypeAttName) == 0)
      {
        name = att->value();
      }

      att = att->next_attribute();
    }

    if (name.empty())
    {
      // TODO: [ERROR] Owned Pointer node must have a "Type" attribute
      name = type->getStaticType()->name;
    }

    const TypeDescriptor* typeDesc = ReflectionHelper::GetTypeDesc(name);

    *ppObj = typeDesc->create();

    if (ptrId == std::numeric_limits<intptr_t>::max())
    {
      // TODO: [ERROR] Owned Pointer node must have a "PrtId" attribute
    }
    else
    {
      ReflectionHelper::RegisterPtrId(ppObj->get(), ptrId, typeDesc->size);
    }

    m_pObj = ppObj->get();
    typeDesc->Accept(this);
  }

  void DeserializationTypeVisitor::Visit(const TypeDescriptor_Asset_Ptr* type)
  {
    /*char* filename = m_xmlNode->value();
    if (filename[0] != '\0') {
      void** ppObj = type->getPPtr(m_pObj);
      const TypeDescriptor* dynamicType = type->GetDynamic(ppObj);
      *ppObj = PrefabManager::GetInstance()->LoadPrefab(filename, dynamicType);
      type->setFilename(m_pObj, filename);
    }*/
  }

  //////////////////// SETUP TYPE VISITOR ///////////////////////

  void ConfigureTypeVisitor::Visit(const TypeDescriptor_Struct* type)
  {
    void* pObj = m_pObj;
    SetupMembers(type);
    type->configure(pObj);
  }

  void ConfigureTypeVisitor::SetupMembers(const TypeDescriptor_Struct* type)
  {
    void* pObj = m_pObj;
    if (type->parentTypeDesc)
    {
      SetupMembers(type->parentTypeDesc);
    }

    int memberCount = type->members.size();

    for (int i = 0; i < memberCount; i++)
    {
      m_pObj = type->members[i].getAddress(pObj);
      type->members[i].type->Accept(this);
    }
  }

  void ConfigureTypeVisitor::Visit(const TypeDescriptor_StdVector* type)
  {
    void* pObj = m_pObj;
    size_t size = type->getSize(pObj);

    for (size_t i = 0; i < size; i++)
    {
      m_pObj = type->getItem(pObj, i);
      type->itemType->Accept(this);
    }
  }

  void ConfigureTypeVisitor::Visit(const TypeDescriptor_Owner_Ptr* type)
  {
    owner_ptr<void>* ppObj = (owner_ptr<void>*)m_pObj;

    if (ppObj->get() == nullptr)
    {
      return;
    }

    const TypeDescriptor* dynamicType = type->getDynamicType(ppObj->get());

    m_pObj = ppObj->get();

    dynamicType->Accept(this);
  }

}