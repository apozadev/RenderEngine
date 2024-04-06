#include "Math/Transform.h"

Transform operator* (const Transform& _rT1, const Transform& _rT2)
{
  return Transform(
    _rT1.m_vPos + _rT2.m_vPos
    , _rT1.m_vScale * _rT2.m_vScale
    , _rT1.m_qRot * _rT2.m_qRot
  );
}