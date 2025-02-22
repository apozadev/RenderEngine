#pragma once 

#include <memory>

#include "Memory/PoolObjectDeleter.h"

template<typename T>
class owner_ptr : public std::unique_ptr<T, PoolObjectDeleter<T>>
{
public:

  using std::unique_ptr<T, PoolObjectDeleter<T>>::unique_ptr;

  owner_ptr(const owner_ptr<T>&) = delete;
  owner_ptr& operator=(const owner_ptr<T>&) = delete;

  // Explicitly allow move semantics
  owner_ptr(owner_ptr<T>&& other) noexcept = default;
  owner_ptr& operator=(owner_ptr<T> && other) noexcept = default;

  template<typename U>
  owner_ptr<U> cast_release()
  {
    return owner_ptr<U>(static_cast<U*>(release()));
  }
};