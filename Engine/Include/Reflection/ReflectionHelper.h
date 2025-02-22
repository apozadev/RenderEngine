#pragma once

#include <unordered_map>
#include <vector>

#include "File/FileUtils.h"
#include "Reflection/SerializationTypeVisitor.h"
#include "Reflection/DeserializationTypeVisitor.h"
#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml.hpp"
  
namespace reflection {

  struct TypeDescriptor;
  struct TypeDescriptor_Struct;

  struct PtrIdPair;

  struct ReflectionPtrInfo;

  class ReflectionHelper {
  public:

    template <typename T>
    static void Serialize(const char* _sFilename, T* _pObj)
    {
      rapidxml::xml_document<> oDoc;

      reflection::SerializationTypeVisitor oVisitor(_pObj, &oDoc);
      T::GetReflection().Accept(&oVisitor);

      std::ofstream myfile;      
      myfile.open((file::GetWorkingDirectory() +  _sFilename).c_str());
      myfile << oDoc;
      myfile.close();

      reflection::ReflectionHelper::ClearTrackedStrings();
    }

    template <typename T>
    static void Deserialize(const char* _sFilename, T* _pObj)
    {
      const reflection::TypeDescriptor_Struct* pTypeDesc = _pObj->GetReflectionDynamic();

      rapidxml::file<> oXmlFile((file::GetWorkingDirectory() + _sFilename).c_str());
      rapidxml::xml_document<> oDoc;
      oDoc.parse<0>(oXmlFile.data());

      const rapidxml::xml_node<>* pNodeElem = oDoc.first_node(pTypeDesc->name);

      reflection::DeserializationTypeVisitor oVisitor(_pObj, pNodeElem);
      pTypeDesc->Accept(&oVisitor);

      reflection::ConfigureTypeVisitor oSetupVisitor(_pObj);
      pTypeDesc->Accept(&oSetupVisitor);
    }

    static void RegisterTypeDesc(TypeDescriptor* typeDesc);

    static const TypeDescriptor* GetTypeDesc(const std::string& name);

    static void TrackString(std::string* string);

    static void ClearTrackedStrings();

    static void RegisterPtrId(void* ptr, intptr_t id, size_t size);

    static void RegisterPendingPtr(void** ptr, intptr_t id);

    static void ResolvePendingPointers();

    static std::vector<const TypeDescriptor_Struct*> GetChildTypes(const TypeDescriptor_Struct* typeDesc);

    static void ClearAll();

  private:
    ReflectionHelper();
    
    typedef std::unordered_map<std::string, TypeDescriptor*> TypeDict;
    typedef std::vector< ReflectionPtrInfo> PtrInfoVector;
    typedef std::vector<std::string*> StrVector;
    typedef std::vector<PtrIdPair> PtrIdVector;

    static TypeDict& GetTypeDict();
    static StrVector& GetStrVector();
    static PtrInfoVector& GetPtrInfoVector();
    static PtrIdVector& GetPendingPtrVector();
  };
}
