#pragma pack_matrix( column_major )

struct VSout
{
  float4 pos : SV_Position;
  float2 uv : TEXCOORD;
  float3 normal : NORMAL;
};

struct PS_OUTPUT
{
  float4 color: SV_Target0;
};

PS_OUTPUT main(VSout i)
{
  PS_OUTPUT o;
  o.color = float4(i.normal, 1);
  return o;
}