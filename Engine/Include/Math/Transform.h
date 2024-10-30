#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Transform
{
public:

  Transform() :
    m_vPos(0.f, 0.f, 0.f)
    , m_vScale(1.f, 1.f, 1.f)
    , m_qRot(1.f, 0.f, 0.f, 0.f)
  {
  }

  Transform(Transform&& _oTransform) :
    m_vPos(_oTransform.m_vPos)
    , m_vScale(_oTransform.m_vScale)
    ,m_qRot(_oTransform.m_qRot)
  {
  }

  Transform(const Transform& _oTransform) :
    m_vPos(_oTransform.m_vPos)
    , m_vScale(_oTransform.m_vScale)
    , m_qRot(_oTransform.m_qRot)
  {
  }

  Transform(glm::vec3&& _vPos, glm::vec3&& _vScale, glm::quat&& _qRot):
    m_vPos(_vPos), m_vScale(_vScale), m_qRot(_qRot)
  {
  }

  Transform(const glm::vec3& _vPos, const glm::vec3& _vScale, const glm::quat& _qRot) :
    m_vPos(_vPos), m_vScale(_vScale), m_qRot(_qRot)
  {
  }

  Transform(glm::mat4&& _mMatrix)
  {
    glm::vec3 oSkew;
    glm::vec4 oPerspective;
    glm::decompose(_mMatrix, m_vScale, m_qRot, m_vPos, oSkew, oPerspective);
  }

  Transform(const glm::mat4& _mMatrix)
  {
    glm::vec3 oSkew;
    glm::vec4 oPerspective;
    glm::decompose(_mMatrix, m_vScale, m_qRot, m_vPos, oSkew, oPerspective);
  }

  void SetPos(const glm::vec3& _vPos) { m_vPos = _vPos; }
  void SetScale(const glm::vec3& _vScale) { m_vScale = _vScale; }
  void SetRot(const glm::quat& _qRot) { m_qRot = _qRot; }

  const glm::vec3& GetPos() const { return m_vPos; }
  const glm::vec3& GetScale() const { return m_vScale; }
  const glm::quat& GetRot() const { return m_qRot; }

  glm::vec3 GetUp() const
  {
    glm::mat4 mMat(GetMatrix());
    return glm::vec3(mMat[1][0], mMat[1][1], mMat[1][2]);
  }

  glm::vec3 GetFront() const
  {
    glm::mat4 mMat(GetMatrix());
    return glm::vec3(mMat[2][0], mMat[2][1], mMat[2][2]);
  }

  glm::vec3 GetRight() const
  {
    glm::mat4 mMat(GetMatrix());
    return glm::vec3(mMat[0][0], mMat[0][1], mMat[0][2]);
  }

  glm::mat4 GetMatrix() const
  {
    glm::mat4 mMat(1.f);    
    mMat = glm::translate(mMat, m_vPos);
    mMat = mMat * glm::toMat4(m_qRot);     
    mMat = glm::scale(mMat, m_vScale);

    return mMat;
  }

  void Translate(const glm::vec3& _vTr) { m_vPos += _vTr; }

  static const Transform& GetIdentity()
  {
    static const Transform oIdentity;
    return oIdentity;
  }

  friend Transform operator* (const Transform& _rT1, const Transform& _rT2);

  void operator=(const Transform& _rT)
  {
    m_vPos = _rT.m_vPos;
    m_vScale = _rT.m_vScale;
    m_qRot = _rT.m_qRot;
  }

private:

  glm::vec3 m_vPos;
  glm::vec3 m_vScale;
  glm::quat m_qRot;
};

Transform operator* (const Transform& _rT1, const Transform& _rT2);
