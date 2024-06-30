#pragma pack_matrix( column_major )

struct VSout
{
  float4 pos : SV_Position;
  float2 uv : TEXCOORD;
  float3 normal : NORMAL;
};

SamplerState texSampler : register(s0);
Texture2D albedoTex : register(t0);

struct PS_OUTPUT
{
  float4 color: SV_Target0;
};

PS_OUTPUT main(VSout i)
{

  float3 lightDir = float3(0, 0, 1);
  float ambientFactor = 0.3;

  float4 color = albedoTex.Sample(texSampler, i.uv);  

  PS_OUTPUT o;
  o.color = color * (max(0, dot(lightDir, normalize(i.normal))) + ambientFactor);
  return o;
}