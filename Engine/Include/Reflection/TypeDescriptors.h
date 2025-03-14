#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#include "Memory/PtrTypes.h"

namespace reflection {

  struct TypeDescriptor;

  class TypeVisitor;

  template <typename T>
  TypeDescriptor* getPrimitiveDescriptor();

  struct DefaultResolver {
    template <typename T> static char func(decltype(&T::GetReflection));
    template <typename T> static int func(...);
    template <typename T>
    struct IsReflected {
      enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
    };

    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* get() {
      return &T::GetReflection();
    }

    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* get() {
      return getPrimitiveDescriptor<T>();
    }

    template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getDynamic(const void* pObj) {
      return ((T*)pObj)->GetReflectionDynamic();
    }

    template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
    static const TypeDescriptor* getDynamic(const void* /* unused */) {
      return getPrimitiveDescriptor<T>();
    }
  };

  template <typename T, typename Enable = void>
  struct TypeResolver {
    static const TypeDescriptor* get() {
      return DefaultResolver::get<T>();
    }
    static const TypeDescriptor* getDynamic(const void* pObj) {
      return DefaultResolver::getDynamic<T>(pObj);
    }
  };

  //--------------------------------------------------------
  // Base class of all type descriptors
  //--------------------------------------------------------

  struct TypeDescriptor {
    const char* name;
    size_t size;

    owner_ptr<void> (*create)();

    TypeDescriptor(const char* name, size_t size) : name{ name }, size{ size }
    {}
    ~TypeDescriptor() {}
    virtual const std::string getFullName() const;
    virtual void Accept(TypeVisitor* visitor) const;
    virtual std::string GetValueStr(const void* obj) const = 0;
    virtual void SetValueFromString(void* pObj, const char* valueCStr) const = 0;    
  };

  //--------------------------------------------------------
  // Type descriptor for user-defined structs/classes (Must derive from GameObject)
  //--------------------------------------------------------

  struct TypeDescriptor_Struct : public TypeDescriptor {
    struct Member {
      const char* name;
      const TypeDescriptor* type;
      void* (*getAddress)(const void*);
    };

    std::vector<Member> members;

    const TypeDescriptor_Struct* parentTypeDesc;
    
    void (*configure)(void*);
    void (*reconfigure)(void*);

    TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor{ nullptr, 0 }
    {      
      init(this);
      std::cout << "Init type: " << name << std::endl;
    }
    TypeDescriptor_Struct(const char* name, size_t size) : TypeDescriptor{ name, size }
    {}

    virtual void Accept(TypeVisitor* visitor) const override;

    int getFirstMemberIdx() const;
    
    bool CanUpCast(const TypeDescriptor_Struct* type) const;
    
  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
  };

  //--------------------------------------------------------
  // Type descriptor for enums
  //--------------------------------------------------------

  struct TypeDescriptor_Enum : public TypeDescriptor {

    struct EnumValue {
      const char* name;
      int value;
    };

    std::vector<EnumValue> values;

    TypeDescriptor_Enum(void (*init)(TypeDescriptor_Enum*)) : TypeDescriptor{ nullptr, 0 } {
      init(this);
    }
    TypeDescriptor_Enum(const char* name) : TypeDescriptor{ name, sizeof(int)} {}

    virtual void Accept(TypeVisitor* visitor) const override;

    std::string GetValueStr(const void* obj) const override;
    void SetValueFromString(void* pObj, const char* valueCStr) const override;
  };

  //--------------------------------------------------------
  // Type descriptor for enum bitmasks
  //--------------------------------------------------------

  struct TypeDescriptor_Bitmask : public TypeDescriptor {

    TypeDescriptor_Enum* enumType;

    TypeDescriptor_Bitmask(void (*init)(TypeDescriptor_Bitmask*)) : TypeDescriptor{ nullptr, 0 } {
      init(this);
    }
    TypeDescriptor_Bitmask() : TypeDescriptor{ "", sizeof(int) } {}

    virtual void Accept(TypeVisitor* visitor) const override;

    std::string GetValueStr(const void* obj) const override;
    void SetValueFromString(void* pObj, const char* valueCStr) const override;
  };

  template<typename T>
  struct BitmaskWrapper {
    int m_value;
    BitmaskWrapper() {}
    BitmaskWrapper(int value) { m_value = value; }
    operator int() const { return m_value; }
  };

#define BITMASK(TYPE) ::reflection::BitmaskWrapper<TYPE> 

  //--------------------------------------------------------
  // Type descriptor for pointer to struct/class
  //--------------------------------------------------------

  constexpr const char* __PtrIdAttName = "PtrId";
  constexpr const char* __PtrTypeAttName = "Type";

  struct TypeDescriptor_Ptr : public TypeDescriptor {

    TypeDescriptor_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor{ nullptr, 0 }
    {
      init(this);
    }
    TypeDescriptor_Ptr(const char* name, size_t size) : TypeDescriptor{ nullptr, 0 }
    {}

    const TypeDescriptor* (*getStaticType)();
    const TypeDescriptor* (*getDynamicType)(const void*);    

  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
  };

  struct TypeDescriptor_Owner_Ptr : public TypeDescriptor_Ptr {

    TypeDescriptor_Owner_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor_Ptr{ init }
    {}
    TypeDescriptor_Owner_Ptr(const char* name, size_t size) : TypeDescriptor_Ptr{ nullptr, 0 }
    {}

    virtual void Accept(TypeVisitor* visitor) const override;

  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
  };

  struct TypeDescriptor_Weak_Ptr : public TypeDescriptor_Ptr {

    TypeDescriptor_Weak_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor_Ptr{ init }
    {}
    TypeDescriptor_Weak_Ptr(const char* name, size_t size) : TypeDescriptor_Ptr{ nullptr, 0 }
    {}

    virtual void Accept(TypeVisitor* visitor) const override;
   
  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
  };

  struct TypeDescriptor_Asset_Ptr : public TypeDescriptor_Ptr {

    std::string (*getFilename)(const void*);
    void (*setFilename)(void*, const std::string&);
    void (*loadPrefab) (void*);
    void** (*getPPtr)(void*);
    const void** (*getPPtrConst)(const void*);

    TypeDescriptor_Asset_Ptr(void (*init)(TypeDescriptor_Ptr*)) : TypeDescriptor_Ptr{ init }
    {}
    TypeDescriptor_Asset_Ptr(const char* name, size_t size) : TypeDescriptor_Ptr{ nullptr, 0 }
    {}

    virtual void Accept(TypeVisitor* visitor) const override;
   
    std::string GetValueStr(const void* obj) const override { return getFilename(obj); }
    void SetValueFromString(void* pObj, const char* valueCStr) const override { setFilename(pObj, valueCStr); }
  };


  template <typename T>
  struct Ptr_Wrapper {
    T* m_ptr = nullptr;

    T* operator->() const { return m_ptr; }
    operator T* () const { return m_ptr; }
    const T& operator* () const { return *m_ptr; }
    T& operator* () { return *m_ptr; }
    T* operator+(int offset) { return m_ptr + offset; }
    T* operator++() { return m_ptr++; }
    T* operator-(int offset) { return m_ptr - offset; }
    T* operator--() { return m_ptr--; }
  protected:
    Ptr_Wrapper() {}
    Ptr_Wrapper(T* ptr) : m_ptr(ptr) {}
  };
  /*template <typename T>
  struct Owned_Ptr_Wrapper : public Ptr_Wrapper<T> {
    Owned_Ptr_Wrapper() {}
    Owned_Ptr_Wrapper(T* ptr) : Ptr_Wrapper<T>(ptr) {}
  };*/
  template <typename T>
  struct Weak_Ptr_Wrapper : public Ptr_Wrapper<T> {
    Weak_Ptr_Wrapper() {}
    Weak_Ptr_Wrapper(T* ptr) : Ptr_Wrapper<T>(ptr) {}
  };
  template <typename T>
  struct Asset_Ptr_Wrapper : public Ptr_Wrapper<T> {
    Asset_Ptr_Wrapper() {}
    Asset_Ptr_Wrapper(T* ptr) : Ptr_Wrapper<T>(ptr) {}   
    std::string m_filename;
  };

#define WEAK_PTR(TYPE) ::reflection::Weak_Ptr_Wrapper<TYPE>
//#define OWNED_PTR(TYPE) ::reflection::Owned_Ptr_Wrapper<TYPE>
#define ASSET_PTR(TYPE) ::reflection::Asset_Ptr_Wrapper<TYPE>

  //--------------------------------------------------------
  // Type descriptors for std::vector
  //--------------------------------------------------------

  struct TypeDescriptor_StdVector : TypeDescriptor {
    const TypeDescriptor* itemType;
    size_t(*getSize)(const void*);
    void* (*getItem)(const void*, size_t);
    void (*resize) (void*, size_t);
    //void (*pushBack) (void*, void*);
    void (*construct) (void*);
    //void (*remove) (void*, unsigned int);

    template <typename ItemType>
    TypeDescriptor_StdVector(ItemType*) :
      TypeDescriptor {
      "std::vector<>", sizeof(std::vector<ItemType>)
    },
      itemType{ TypeResolver<ItemType>::get() }
      {
        getSize = [](const void* vecPtr) -> size_t {
          const auto& vec = *(const std::vector<ItemType>*) vecPtr;
          return vec.size();
        };
        getItem = [](const void* vecPtr, size_t index) -> void* {
          auto& vec = *(std::vector<ItemType>*) vecPtr;
          return &(vec[index]);
        };
        resize = [](void* vecPtr, size_t size) {
          auto vec = (std::vector<ItemType>*) vecPtr;

          //if constexpr (std::is_copy_constructible_v<ItemType>)
          {
            // Copy-constructible types can use normal resize()
            vec->resize(size);
          }
        };/*
        pushBack = [](void* vecPtr, void* valuePtr) {
          auto vec = (std::vector<ItemType>*) vecPtr;
          vec->push_back(*(ItemType*)valuePtr);
        };*/
        construct = [](void* obj) {
          new(obj) std::vector<ItemType>();
        };/*
        remove = [](void* vecPtr, unsigned int idx) {
          auto vec = (std::vector<ItemType>*) vecPtr;
          vec->erase(vec->begin() + idx);
        };*/
      }

    virtual void Accept(TypeVisitor* visitor) const override;

    virtual const std::string getFullName() const override;
   
  private:
    std::string GetValueStr(const void* obj) const override { return {}; }
    void SetValueFromString(void* pObj, const char* valueCStr) const override {}
  };

  //--------------------------------------------------------
  // Type descriptor for struct/class we want to hide from visitors
  //--------------------------------------------------------

  class TypeDescriptor_Ignored : public TypeDescriptor_Struct {
  public:
    //TypeDescriptor_Ignored() : TypeDescriptor_Struct("", (size_t)0) {}
    TypeDescriptor_Ignored(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor_Struct{ init } {}
    TypeDescriptor_Ignored(const char* name, size_t size) : TypeDescriptor_Struct{ name, size }{}
    ~TypeDescriptor_Ignored() {}
    virtual void Accept(TypeVisitor* visitor) const override;
    virtual std::string GetValueStr(const void* obj) const override { return ""; }
    virtual void SetValueFromString(void* pObj, const char* valueCStr) const override {}
  };

  template <typename T>
  class TypeResolver<std::vector<T>, typename std::enable_if_t<!std::is_pointer<T>::value, int>::type> {
  public:
    static TypeDescriptor* get() {
      static TypeDescriptor_StdVector typeDesc{ (T*) nullptr };
      return &typeDesc;
    }
  };

  template <typename T>
  class TypeResolver<std::vector<T>, typename std::enable_if_t<std::is_pointer<T>::value, int>::type> {
  public:
    static TypeDescriptor* get() {
      static TypeDescriptor_StdVector typeDesc{ (reflection::Weak_Ptr_Wrapper<T>*) nullptr };
      return &typeDesc;
    }
  };

  template <typename T>
  class TypeResolver<std::vector<owner_ptr<T>>> {
  public:
    static TypeDescriptor* get() {
      static TypeDescriptor_StdVector typeDesc{ (owner_ptr<T>*) nullptr };
      return &typeDesc;
    }
  };


} // namespace reflect
