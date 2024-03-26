#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:

  Transform() :
    m_vPos(0.f, 0.f, 0.f)
    , m_vScale(1.f, 1.f, 1.f)
    , m_qRot(1.f, 0.f, 0.f, 0.f)
  {
  }

  void SetPos(const glm::vec3& _vPos) { m_vPos = _vPos; }
  void SetScale(const glm::vec3& _vScale) { m_vScale = _vScale; }
  void SetRot(const glm::quat& _qRot) { m_qRot = _qRot; }

  const glm::vec3& GetPos() const { return m_vPos; }
  const glm::vec3& GetScale() const { return m_vScale; }
  const glm::quat& GetRot() const { return m_qRot; }

  glm::mat4x4 GetMatrix() const
  {
    glm::mat4x4 mMat(1.f);
    mMat = glm::translate(mMat, m_vPos);
    mMat = glm::scale(mMat, m_vScale);
    //mMat = glm::toMat4(m_qRot) * mMat;

    return mMat;
  }

  void Translate(const glm::vec3& _vTr) { m_vPos += _vTr; }

private:

  glm::vec3 m_vPos;
  glm::vec3 m_vScale;
  glm::quat m_qRot;
};
