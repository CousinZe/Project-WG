#include "cwglx/drawable/TriangleGenerator.h"

namespace cw {

TriangleGenerator::~TriangleGenerator() = default;

SimpleTriangle::SimpleTriangle(const std::array<Vertex, 3>& vertices)
  : m_Vertices(vertices),
    m_Generated(false)
{}

SimpleTriangle::~SimpleTriangle() = default;

bool SimpleTriangle::HasNextTriangle() {
  return m_Generated;
}

std::array<Vertex, 3> SimpleTriangle::NextTriangle() {
  m_Generated = false;
  return m_Vertices;
}

SimpleTriangle SimpleTriangle::Clone() noexcept {
  return SimpleTriangle { m_Vertices };
}

Positioner::Positioner(std::unique_ptr<TriangleGenerator> &&generator,
                       const Vector& position)
  : m_Generator(std::move(generator)),
    m_Position(position)
{}

Positioner::~Positioner() {}

bool Positioner::HasNextTriangle() {
  return m_Generator->HasNextTriangle();
}

std::array<Vertex, 3> Positioner::NextTriangle() {
  auto triangle = m_Generator->NextTriangle();
  for (auto& vertex : triangle) {
    vertex += m_Position;
  }
  return triangle;
}

Composer::Composer(std::vector<std::unique_ptr<TriangleGenerator>> &&generators)
  : m_Generators(std::move(generators))
{}

Composer::~Composer() = default;

bool Composer::HasNextTriangle() {
  while (!m_CurrentGenerator->get()->HasNextTriangle()
         && m_CurrentGenerator == m_Generators.cend()) {
    ++m_CurrentGenerator;
  }

  return m_CurrentGenerator != m_Generators.cend();
}

std::array<Vertex, 3> Composer::NextTriangle() {
  while (!m_CurrentGenerator->get()->HasNextTriangle()) {
    ++m_CurrentGenerator;
  }

  return m_CurrentGenerator->get()->NextTriangle();
}

} // namespace cw
