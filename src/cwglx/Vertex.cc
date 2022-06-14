#include "cwglx/Vertex.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

Vertex2D::Vertex2D(GLdouble x, GLdouble y) noexcept
  : m_Repr({x, y})
{}

Vertex::Vertex(GLdouble x, GLdouble y, GLdouble z) noexcept
  : m_Repr({x, y, z})
{}

Vector Vector::FromVertex(const Vertex &vertex) noexcept {
  return Vector { vertex.GetX(), vertex.GetY(), vertex.GetZ() };
}

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

Vector Vector::Normalize() const noexcept {
  const double length = std::sqrt(GetX() * GetX()
                                  + GetY() * GetY()
                                  + GetZ() * GetZ());
  return Vector(GetX() / length, GetY() / length, GetZ() / length);
}

Vector Vector::ABS() const noexcept {
  return Vector(std::abs(GetX()), std::abs(GetY()), std::abs(GetZ()));
}

Vertex Vector::AsVertex() const noexcept {
  return Vertex { GetX(), GetY(), GetZ() };
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

Vector operator-(const Vertex& lhs, const Vertex& rhs) noexcept {
  return Vector(lhs.GetX() - rhs.GetX(),
                lhs.GetY() - rhs.GetY(),
                lhs.GetZ() - rhs.GetZ());
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

void DrawVertexArray(QOpenGLFunctions_2_0 *f,
                     const VertexF *vertices,
                     GLsizei count) noexcept
{
  f->glEnableClientState(GL_VERTEX_ARRAY);
  f->glVertexPointer(3, GL_FLOAT, 0, vertices);
  f->glDrawArrays(GL_TRIANGLES, 0, count);
}

Vertex RotateVertex(const Vertex &vertex,
                    const Vertex &centerPoint,
                    Axis axis,
                    GLdouble degree)
{
  Vector diff = vertex - centerPoint;
  const auto [x, y, z] = diff.GetRepr();
  double cos = std::cos(degree);
  double sin = std::sin(degree);
  switch (axis) {
    case Axis::XAxis:
      return Vertex(x,
                    y * cos - z * sin,
                    y * sin + z * cos);
    case Axis::YAxis:
      return Vertex(x * cos + z * sin,
                    y,
                    -x * sin + z * cos);
    case Axis::ZAxis:
      return Vertex(x * cos - y * sin,
                    x * sin + y * cos,
                    z);
  }

  Q_UNREACHABLE();
}

namespace constants {

Vertex g_ZeroVertex {0.0, 0.0, 0.0};

Vector g_ZeroVector {0.0, 0.0, 0.0};
Vector g_UnitVectorX {1.0, 0.0, 0.0};
Vector g_UnitVectorY {0.0, 1.0, 0.0};
Vector g_UnitVectorZ {0.0, 0.0, 1.0};

} // namespace constants

} // namespace cw

