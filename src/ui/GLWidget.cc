#include "ui/GLWidget.h"

#include <experimental/array>
#include <QTimer>
#include <QTimerEvent>

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"
#include "cwglx/Material.h"
#include "cwglx/drawable/Composition.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGen.h"
#include "wgc0310/head/Head.h"
#include "wgc0310/head/ScreenGlass.h"
#include "wgc0310/head/Screen.h"
#include "ui/ConfigWidget.h"

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    GLFunctions(),
    m_Light(nullptr),
    m_Light2(nullptr),
    m_Arena(),
    m_HeadId(0),
    m_ScreenGlassId(0),
    m_ScreenId(0)
{
  QSurfaceFormat format;
  format.setSamples(8);
  format.setDepthBufferSize(16);
  format.setBlueBufferSize(0);
  format.setGreenBufferSize(0);
  format.setRedBufferSize(0);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

  this->setFormat(format);

  this->setAttribute(Qt::WA_TranslucentBackground);
  this->setAttribute(Qt::WA_TransparentForMouseEvents);
  this->setAttribute(Qt::WA_AlwaysStackOnTop);

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &GLWidget::updateRotation);
  timer->setInterval(10);
  timer->start();

  int timerId = this->startTimer(10);
  if (timerId == 0) {
    qDebug() << "could not start timer correctly";
  }

  m_ConfigWidget = new ConfigWidget(&this->m_CameraEntityStatus);

  setBaseSize(1024, 768);
  resize(1024,768);
}

GLWidget::~GLWidget() {
  makeCurrent();

  m_Arena.Delete(this);
  qDebug() << "GLWidget::~GLWidget(): arena released successfully";

  doneCurrent();
}

void GLWidget::updateRotation() {
  this->update();
}

void GLWidget::initializeGL() {
  cw::SetupPreferredSettings(this);
  m_Light.reset(new cw::PointLight(GL_LIGHT0,
                                   cw::RGBAColor(32, 32, 32),
                                   cw::RGBAColor(127, 127, 127),
                                   cw::RGBAColor(255, 255, 255),
                                   cw::Vertex(-25.0, 0.0, 0),
                                   this));
  m_Light2.reset(new cw::PointLight(GL_LIGHT1,
                                    cw::RGBAColor(32, 32, 32),
                                    cw::RGBAColor(127, 127, 127),
                                    cw::RGBAColor(255, 255, 255),
                                    cw::Vertex(25.0, 0.0, 0),
                                    this));

  std::unique_ptr<cw::TriangleGen> headGenerator = wgc0310::Head();
  std::unique_ptr<cw::PlainTriangles> headTriangles =
      std::make_unique<cw::PlainTriangles>();
  headTriangles->AddTriangles(headGenerator.get());
  headTriangles->PreInitialize(this);

  const auto [_, head] = m_Arena.Put(std::move(headTriangles));
  const auto [headId, _2] = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
          cw::GetBrassMaterial(),
          std::vector { head }
      )
  );
  m_HeadId = headId;

  std::unique_ptr<cw::TriangleGen> glassGenerator = wgc0310::ScreenGlass();
  std::unique_ptr<cw::PlainTriangles> glassTriangles =
      std::make_unique<cw::PlainTriangles>();
  glassTriangles->AddTriangles(glassGenerator.get());
  glassTriangles->PreInitialize(this);

  const auto [_3, glass] = m_Arena.Put(std::move(glassTriangles));
  const auto [glassId, _4] = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
          cw::GetGlassMaterial(),
          std::vector { glass }
      )
  );
  m_ScreenGlassId = glassId;

  const auto [screenId, _5] = m_Arena.Put(
      std::make_unique<wgc0310::Screen>(this)
  );
  m_ScreenId = screenId;

  cw::GLInfo glInfo = cw::GLInfo::AutoDetect(this);
  m_ConfigWidget->FillGLInfo(glInfo);
}

void GLWidget::paintGL() {
  // start timer
  const auto start = std::chrono::high_resolution_clock::now();

  // preparation stage
  wgc0310::Screen const* screen =
      static_cast<wgc0310::Screen const*>(m_Arena.Get(m_ScreenId));
  screen->PrepareTexture(this);

  // switch back to default framebuffer. not using `0` since
  // Qt doesn't use it as the default one.
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

  // painting stage
  GLsizei w = static_cast<GLsizei>(width());
  GLsizei h = static_cast<GLsizei>(height());
  GLdouble wd = static_cast<GLdouble>(w);
  GLdouble hd = static_cast<GLdouble>(h);

  glViewport(0.0f, 0.0f, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glu::Perspective(this, 45.0, wd / hd, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(0.01f, 0.01f, 0.01f);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_Light->Enable(this);
  m_Light2->Enable(this);

  {
    glTranslatef(-m_CameraEntityStatus.cameraX,
                 -m_CameraEntityStatus.cameraY,
                 -m_CameraEntityStatus.cameraZ);
    cw::Vector cameraRotation {
        -m_CameraEntityStatus.cameraRotateX,
        -m_CameraEntityStatus.cameraRotateY,
        -m_CameraEntityStatus.cameraRotateZ
    };
    cw::Vector normalizedCRotationVec = cameraRotation.Normalize();
    GLdouble rotationCVecLen = cameraRotation.Length();
    if (rotationCVecLen >= 0.0005) {
      glRotatef(static_cast<GLfloat>(rotationCVecLen),
                static_cast<GLfloat>(normalizedCRotationVec.GetX()),
                static_cast<GLfloat>(normalizedCRotationVec.GetY()),
                static_cast<GLfloat>(normalizedCRotationVec.GetZ()));
    }
  }

  {
    glTranslatef(m_CameraEntityStatus.entityX,
                 m_CameraEntityStatus.entityY,
                 m_CameraEntityStatus.entityZ);
    cw::Vector entityRotation {
        m_CameraEntityStatus.entityRotateX,
        m_CameraEntityStatus.entityRotateY,
        m_CameraEntityStatus.entityRotateZ
    };
    cw::Vector normalizedRotationVec = entityRotation.Normalize();
    GLdouble rotationVecLen = entityRotation.Length();
    if (rotationVecLen >= 0.0005) {
      glRotatef(static_cast<GLfloat>(rotationVecLen),
                static_cast<GLfloat>(normalizedRotationVec.GetX()),
                static_cast<GLfloat>(normalizedRotationVec.GetY()),
                static_cast<GLfloat>(normalizedRotationVec.GetZ()));
    }
  }

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  m_Arena.Get(m_HeadId)->Draw(this);

  screen->Draw(this);

  glTranslatef(0.0f, 0.0f, 0.5f);
  glPushAttrib(GL_CURRENT_COLOR);
  glColor4f(0.05f, 0.075f, 0.1f, 0.1f);
  m_Arena.Get(m_ScreenGlassId)->Draw(this);
  glPopAttrib();

  // calculate execution time
  const auto end = std::chrono::high_resolution_clock::now();
  const auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  qDebug() << "paintGL took" << duration.count() << "us";
}

void GLWidget::resizeGL(int w, int h) {
  Q_UNUSED(w)
  Q_UNUSED(h)
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  Q_UNUSED(event)

  m_ConfigWidget->show();
}
