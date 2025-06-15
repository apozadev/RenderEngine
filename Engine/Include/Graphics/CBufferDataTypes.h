
struct DirLightData
{
  glm::vec4 m_vDir;
  glm::vec4 m_vColor;
};

struct alignas(16) LightData
{
  DirLightData m_aLights[MAX_LIGHTS];
  glm::mat4 m_aLightViews[MAX_LIGHTS];
  unsigned int m_uNumLights;
  unsigned int m_uNumShadows;
  float pad0, pad1;
};

struct GlobalBufferData
{
  glm::mat4 m_mViewProj;
  glm::mat4 m_mView;
  glm::mat4 m_mViewInv;
  glm::mat4 m_mProj;
};
