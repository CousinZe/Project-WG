#include "wgc0310/Mesh.h"

#include "cwglx/GL/GLImpl.h"
#include "cwglx/Base/Shader.h"
#include "cwglx/Object/WavefrontLoader.h"
#include "util/FileUtil.h"

namespace wgc0310 {

SimpleObject::SimpleObject(GLFunctions *f, cw::SimpleVertex *vertices, std::size_t vertexCount)
  : m_VAO(f),
    m_VBO(f),
    m_VertexCount(static_cast<GLsizei>(vertexCount))
{
  m_VAO.Bind(f);
  m_VBO.Bind(f);
  m_VBO.BufferData(f, vertices, vertexCount);

  m_VAO.Unbind(f);
  m_VBO.Unbind(f);
}

void SimpleObject::Draw(GLFunctions *f, bool unbind) {
  m_VAO.Bind(f);
  f->glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
  if (unbind) {
    m_VAO.Unbind(f);
  }
}

void SimpleObject::Delete(GLFunctions *f) {
  m_VAO.Delete(f);
  m_VBO.Delete(f);
}

StrokeObject::StrokeObject(GLFunctions *f, cw::PlainVertex *vertices, std::size_t vertexCount)
  : m_VAO(f),
    m_VBO(f),
    m_VertexCount(static_cast<GLsizei>(vertexCount))
{
  m_VAO.Bind(f);
  m_VBO.Bind(f);
  m_VBO.BufferData(f, vertices, vertexCount);

  m_VAO.Unbind(f);
  m_VBO.Unbind(f);
}

void StrokeObject::Draw(GLFunctions *f, bool unbind) {
  m_VAO.Bind(f);
  f->glDrawArrays(GL_LINES, 0, m_VertexCount);
  if (unbind) {
    m_VAO.Unbind(f);
  }
}

void StrokeObject::Delete(GLFunctions *f) {
  m_VAO.Delete(f);
  m_VBO.Delete(f);
}

std::unique_ptr<WGCMeshCollection> LoadWGCMesh(GLFunctions *f) {
  auto monitor = cw::LoadMesh("./model/monitor.mesh");
  auto monitorIntake = cw::LoadMesh("./model/monitor-intake.mesh");
  auto monitorStroke = cw::LoadLineMesh("./model/stroke/monitor.mesh");

  return std::unique_ptr<WGCMeshCollection>(new WGCMeshCollection { // NOLINT(modernize-make-unique)
    .monitor = SimpleObject(f, monitor.data(), monitor.size()),
    .monitorIntake = SimpleObject(f, monitorIntake.data(), monitorIntake.size()),
    .monitorStroke = StrokeObject(f, monitorStroke.data(), monitorStroke.size()),
  });
}

void WGCMeshCollection::Delete(GLFunctions *f) {
  monitor.Delete(f);
  monitorIntake.Delete(f);
  monitorStroke.Delete(f);
}

} // namespace wgc0310
