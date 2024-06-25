#pragma once

namespace api
{
namespace dx11
{

  enum VsBindSlotOffset
  {
    VS_SLOT_OFFSET_GLOBAL = 0,
    VS_SLOT_OFFSET_MATERIAL = 1,
    VS_SLOT_OFFSET_MATINSTANCE = 2,
    VS_SLOT_OFFSET_FREE = 3
  };

  enum PsBindSlotOffset
  {
    PS_SLOT_OFFSET_GLOBAL = 0,
    PS_SLOT_OFFSET_MATERIAL = 1,
    PS_SLOT_OFFSET_MATINSTANCE = 2,
    PS_SLOT_OFFSET_FREE = 3
  };

}
}