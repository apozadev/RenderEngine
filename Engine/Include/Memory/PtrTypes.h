#pragma once 

#include <memory>

#include "Memory/PoolObjectDeleter.h"

template<typename T>
using owner_ptr = std::unique_ptr<T, PoolObjectDeleter<T>>;