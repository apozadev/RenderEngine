#pragma once

namespace api
{
namespace dx11
{

  enum VsBindSlotOffset
  {
    VS_SLOT_OFFSET_GLOBAL = 0,
    VS_SLOT_OFFSET_STEP = 1,
    VS_SLOT_OFFSET_MATERIAL = 3,
    VS_SLOT_OFFSET_MATINSTANCE = 3,
    VS_SLOT_OFFSET_FREE = 4
  };

  enum PsBindSlotOffset
  {
    PS_SLOT_OFFSET_GLOBAL = 0,
    PS_SLOT_OFFSET_STEP = 1,
    PS_SLOT_OFFSET_MATERIAL = 2,
    PS_SLOT_OFFSET_MATINSTANCE = 3,
    PS_SLOT_OFFSET_FREE = 4
  };

}
}