#include <cmath>

#include "geometry.hh"

namespace Geometry {

Matrix3x3
Matrix3x3::identity() {
  Matrix3x3 I;
  I.m_.fill(0.0);
  I.m_[0] = 1.0;
  I.m_[4] = 1.0;
  I.m_[8] = 1.0;
  return I;
}

Matrix3x3
Matrix3x3::rotation(const Vector3D &axis, double angle) {
  Matrix3x3 A, B;
  A.m_[0] = 0.0;      A.m_[1] = -axis[2]; A.m_[2] = axis[1];
  A.m_[3] = axis[2];  A.m_[4] = 0.0;      A.m_[5] = -axis[0];
  A.m_[6] = -axis[1]; A.m_[7] = axis[0];  A.m_[8] = 0.0;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      B.m_[3*i+j] = axis[i] * axis[j];
  return identity() * std::cos(angle) + A * std::sin(angle) + B * (1.0 - std::cos(angle));
}

Matrix3x3
Matrix3x3::operator+(const Matrix3x3 &m) const {
  Matrix3x3 result;
  for (size_t i = 0; i < 9; ++i)
    result.m_[i] = m_[i] + m.m_[i];
  return result;
}

Matrix3x3 &
Matrix3x3::operator+=(const Matrix3x3 &m) {
  for (size_t i = 0; i < 9; ++i)
    m_[i] += m.m_[i];
  return *this;
}

Matrix3x3
Matrix3x3::operator*(double x) const {
  Matrix3x3 result;
  for (size_t i = 0; i < 9; ++i)
    result.m_[i] = m_[i] * x;
  return result;
}

Matrix3x3 &
Matrix3x3::operator*=(double x) {
  for (size_t i = 0; i < 9; ++i)
    m_[i] *= x;
  return *this;
}

Vector3D
Matrix3x3::operator*(const Vector3D &v) const {
  Vector3D result;
  for (size_t i = 0; i < 3; ++i) {
    double sum = 0.0;
    for (size_t k = 0; k < 3; ++k)
      sum += m_[3*i+k] * v[k];
    result[i] = sum;
  }
  return result;
}

Matrix3x3
Matrix3x3::operator*(const Matrix3x3 &m) const {
  Matrix3x3 result;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j) {
      double sum = 0.0;
      for (size_t k = 0; k < 3; ++k)
        sum += m_[3*i+k] * m.m_[3*k+j];
      result.m_[3*i+j] = sum;
    }
  return result;
}

Matrix3x3 &
Matrix3x3::operator*=(const Matrix3x3 &m) {
  *this = (*this) * m;
  return *this;
}

} // namespace Geometry