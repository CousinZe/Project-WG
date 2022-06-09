#include "cwglx/Vertex.h"

namespace cw {

Vertex2D::Vertex2D(GLdouble x, GLdouble y) noexcept
  : m_Repr({x, y})
{}

Vertex::Vertex(GLdouble x, GLdouble y, GLdouble z) noexcept
  : m_Repr({x, y, z})
{}

Vertex &Vertex::operator+=(const Vector &rhs) noexcept {
  m_Repr[0] += rhs.GetX();
  m_Repr[1] += rhs.GetY();
  m_Repr[2] += rhs.GetZ();
  return *this;
}

Vertex &Vertex::operator-=(const Vector &rhs) noexcept {
  m_Repr[0] -= rhs.GetX();
  m_Repr[1] -= rhs.GetY();
  m_Repr[2] -= rhs.GetZ();
  return *this;
}

Vector::Vector(GLdouble x, GLdouble y, GLdouble z) noexcept
  : m_Repr({x, y, z})
{}

Vector &Vector::operator+=(const Vector &rhs) noexcept {
  m_Repr[0] += rhs.GetX();
  m_Repr[1] += rhs.GetY();
  m_Repr[2] += rhs.GetZ();
  return *this;
}

Vector &Vector::operator-=(const Vector &rhs) noexcept {
  m_Repr[0] -= rhs.GetX();
  m_Repr[1] -= rhs.GetY();
  m_Repr[2] -= rhs.GetZ();
  return *this;
}

Vector &Vector::operator*=(GLdouble rhs) noexcept {
  m_Repr[0] *= rhs;
  m_Repr[1] *= rhs;
  m_Repr[2] *= rhs;
  return *this;
}

Vector operator+(const Vector& lhs, const Vector& rhs) noexcept {
  return Vector(lhs.GetX() + rhs.GetX(),
                lhs.GetY() + rhs.GetY(),
                lhs.GetZ() + rhs.GetZ());
}

Vector operator-(const Vector& lhs, const Vector& rhs) noexcept {
  return Vector(lhs.GetX() - rhs.GetX(),
                lhs.GetY() - rhs.GetY(),
                lhs.GetZ() - rhs.GetZ());
}

Vector operator*(const Vector& lhs, GLdouble rhs) noexcept {
  Vector ret = lhs;
  ret *= rhs;
  return ret;
}

Vector operator*(GLdouble lhs, const Vector& rhs) noexcept {
  return rhs * lhs;
}

Vector operator*(const Vector& lhs, const Vector& rhs) noexcept {
  const auto& [l, m, n] = lhs.GetRepr();
  const auto& [o, p, q] = rhs.GetRepr();

  return Vector(m * q - n * p,
                n * o - l * q,
                l * p - m * o);
}

Vertex operator+(const Vertex& lhs, const Vector& rhs) noexcept {
  Vertex ret = lhs;
  ret += rhs;
  return ret;
}

Vertex operator-(const Vertex& lhs, const Vector& rhs) noexcept {
  Vertex ret = lhs;
  ret -= rhs;
  return ret;
}

Vertex operator+(const Vector& lhs, const Vertex& rhs) noexcept {
  return rhs + lhs;
}

Vertex operator-(const Vector& lhs, const Vertex& rhs) noexcept {
  return rhs - lhs;
}

Vertex2DF::Vertex2DF(GLfloat x, GLfloat y) noexcept
  : m_Repr({x, y})
{}

Vertex2DF Vertex2DF::Downscale(const Vertex2D &v) noexcept {
  return Vertex2DF { static_cast<GLfloat>(v.GetX()),
                     static_cast<GLfloat>(v.GetY()) };
}

VertexF::VertexF(GLfloat x, GLfloat y, GLfloat z) noexcept
  : m_Repr({x, y, z})
{}

VertexF VertexF::Downscale(const Vertex &v) noexcept {
  return VertexF {
    static_cast<GLfloat>(v.GetX()),
    static_cast<GLfloat>(v.GetY()),
    static_cast<GLfloat>(v.GetZ())
  };
}

VectorF::VectorF(GLfloat x, GLfloat y, GLfloat z) noexcept
  : m_Repr({x, y, z})
{}

VectorF VectorF::Downscale(const Vector &v) noexcept {
  return VectorF {
    static_cast<GLfloat>(v.GetX()),
    static_cast<GLfloat>(v.GetY()),
    static_cast<GLfloat>(v.GetZ())
  };
}

} // namespace cw
