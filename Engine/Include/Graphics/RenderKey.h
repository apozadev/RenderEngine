#pragma once

#include <bitset>

// Opaque                                                           
//------------------------------------------------------------------------------------
// 127       | 123      | 107     | 91              | 75        | 11
// -----------------------------------------------------------------------------------
// 4 window  | 16 layer | 16 pass | 16 mat instance | 64 depth  | 12 unused

// Translucent                                                      
//------------------------------------------------------------------------------------
// 127      | 123       | 107       | 43      | 27              | 11
// -----------------------------------------------------------------------------------
// 4 window | 16 layer  | 64 depth  | 16 pass | 16 mat instance | 12 unused

#define KEY_SIZE 128

typedef std::bitset<KEY_SIZE> RenderKey;

#define GEN_KEY(value, size, pos) (static_cast<RenderKey>(value) << (pos-size))

#define SET_KEY_WINDOW(key, value)       key |= GEN_KEY(value, 4,  KEY_SIZE)
#define SET_KEY_LAYER(key, value)        key |= GEN_KEY(value, 16, KEY_SIZE-4)
#define SET_KEY_PASS(key, value)         key |= GEN_KEY(value, 16, KEY_SIZE-4-16)
#define SET_KEY_MATINSTANCE(key, value)  key |= GEN_KEY(value, 16, KEY_SIZE-4-16-16)
#define SET_KEY_DEPTH(key, value)        key |= GEN_KEY(value, 64, KEY_SIZE-4-16-16-16)

#define SET_KEY_WINDOW_TRANSLUCENT(key, value)       key |= GEN_KEY(value, 4,  KEY_SIZE)
#define SET_KEY_LAYER_TRANSLUCENT(key, value)        key |= GEN_KEY(value, 16, KEY_SIZE-4)
#define SET_KEY_DEPTH_TRANSLUCENT(key, value)        key |= GEN_KEY(value, 64, KEY_SIZE-4-16)
#define SET_KEY_PASS_TRANSLUCENT(key, value)         key |= GEN_KEY(value, 16, KEY_SIZE-4-16-64)
#define SET_KEY_MATINSTANCE_TRANSLUCENT(key, value)  key |= GEN_KEY(value, 16, KEY_SIZE-4-16-64-16)

inline bool operator>(const RenderKey& _xKey0, const RenderKey& _xKey1)
{
  for (int i = KEY_SIZE - 1; i >= 0; i--)
  {
    if (_xKey0[i] && !_xKey1[i]) return false;
    if (!_xKey0[i] && _xKey1[i]) return true;
  }
  return false;
}
