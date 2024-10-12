#pragma once

#include <bitset>

// Opaque                                                           
//------------------------------------------------------------------------------------
// 127       | 123        | 115     | 107     | 91              | 75        | 15
// -----------------------------------------------------------------------------------
// 4 window | 8 pipeline  | 8 step  | 16 pass | 16 mat instance | 64 depth  | 16 unused

// Translucent                                                      
//------------------------------------------------------------------------------------
// 127      | 123       | 63          | 55      | 47      | 31              | 15 
// -----------------------------------------------------------------------------------
// 4 window | 32 depth  | 8 pipeline  | 8 step  | 16 pass | 16 mat instance | 16 unused

#define KEY_SIZE 128

typedef std::bitset<KEY_SIZE> RenderKey;

#define GEN_KEY(value, size, pos) (value << (size - pos))

#define SET_KEY_WINDOW(key, value)       key |= GEN_KEY(value, 4,  KEY_SIZE)
#define SET_KEY_PASS(key, value)         key |= GEN_KEY(value, 32, KEY_SIZE-4)
#define SET_KEY_MATINSTANCE(key, value)  key |= GEN_KEY(value, 16, KEY_SIZE-4-32)
#define SET_KEY_DEPTH(key, value)        key |= GEN_KEY(value, 64, KEY_SIZE-4-32-16)

#define SET_KEY_WINDOW_TRANSLUCENT(key, value)       key |= GEN_KEY(value, 4,  KEY_SIZE)
#define SET_KEY_DEPTH_TRANSLUCENT(key, value)        key |= GEN_KEY(value, 64, KEY_SIZE-4)
#define SET_KEY_PASS_TRANSLUCENT(key, value)         key |= GEN_KEY(value, 32, KEY_SIZE-4-64)
#define SET_KEY_MATINSTANCE_TRANSLUCENT(key, value)  key |= GEN_KEY(value, 16, KEY_SIZE-4-64-32)

inline bool operator<(const RenderKey& _xKey0, const RenderKey& _xKey1)
{
  for (int i = KEY_SIZE - 1; i >= 0; i--)
  {
    if (_xKey0[i] && !_xKey1[i]) return false;
    if (!_xKey0[i] && _xKey1[i]) return true;
  }
  return false;
}
