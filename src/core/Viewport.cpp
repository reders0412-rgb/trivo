#include "Viewport.h"
#include "Scene.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <cmath>

Viewport::Viewport(std::shared_ptr<Scene> scene, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_scene(scene)
{
    setFocusPolicy(Qt::StrongFocus);

    // 60 fps target
    connect(&m_frameTimer, &QTimer::timeout, this, &Viewport::onFrameTick);
    m_frameTimer.start(16);

    connect(scene.get(), &Scene::modelAdded, this, &Viewport::onModelAdded);
    connect(scene.get(), &Scene::sceneCleared, this, [this]{ update(); });
}

Viewport::~Viewport() = default;

// ── OpenGL ────────────────────────────────────────────────────────────────────

void Viewport::initializeGL()
{
    initializeOpenGLFunctions();
    m_renderer = std::make_unique<Renderer>();
    m_renderer->initialize();
    m_glReady = true;
}

void Viewport::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Viewport::paintGL()
{
    if (!m_glReady || !m_renderer) return;

    const QColor bg = m_scene->backgroundColor();
    glClearColor(bg.redF(), bg.greenF(), bg.blueF(), 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = width() > 0 ? float(width()) / float(height()) : 1.f;

    m_renderer->render(
        m_scene,
        m_camera.viewMatrix(),
        m_camera.projectionMatrix(aspect),
        m_camera.position(),
        m_lights,
        m_anim.currentTime()
    );
}

// ── Frame tick ────────────────────────────────────────────────────────────────

void Viewport::onFrameTick()
{
    m_anim.tick();
    update();
}

void Viewport::onModelAdded()
{
    // Fit camera to new scene bounds
    const auto &models = m_scene->models();
    if (!models.empty()) {
        // Simple bounding estimate: use center of all model positions
        QVector3D center{0,0,0};
        for (const auto &m : models) center += m->position;
        center /= static_cast<float>(models.size());
        m_camera.fitToScene(center, 3.f);
    }
    update();
}

// ── Controls ──────────────────────────────────────────────────────────────────

void Viewport::resetCamera()
{
    m_camera.reset();
    onModelAdded();
}

void Viewport::toggleAnimation()
{
    if (m_anim.isPlaying()) m_anim.pause();
    else                    m_anim.play();
}

void Viewport::setAnimationSpeed(double s) { m_anim.setSpeed(s); }

void Viewport::setLightPreset(int index)
{
    m_lights.applyPreset(index);
    update();
}

void Viewport::setBackgroundColor(const QColor &c)
{
    m_scene->setBackgroundColor(c);
    update();
}

QColor Viewport::backgroundColor() const { return m_scene->backgroundColor(); }

// ── Mouse ─────────────────────────────────────────────────────────────────────

void Viewport::mousePressEvent(QMouseEvent *e)
{
    // 좌클릭: 모델 선택
    if (e->button() == Qt::LeftButton && m_transformMode == TransformMode::None) {
        m_selectedModelIdx = pickModel(e->pos().x(), e->pos().y());
        if (m_selectedModelIdx >= 0) {
            m_scene->setSelectedModelIndex(m_selectedModelIdx);
            emit m_scene->selectedModelChanged(m_selectedModelIdx);
            update();
            return;
        }
    }
    
    m_lastMousePos = e->pos();
    m_activeButton = e->button();
    setFocus();
}

void Viewport::mouseMoveEvent(QMouseEvent *e)
{
    const QPoint delta = e->pos() - m_lastMousePos;
    m_lastMousePos = e->pos();

    // 변환 모드 처리
    if (m_transformMode != TransformMode::None && m_selectedModelIdx >= 0) {
        auto model = m_scene->models()[m_selectedModelIdx];
        float sensitivity = 0.01f;
        
        if (m_transformMode == TransformMode::Move && e->buttons() & Qt::LeftButton) {
            // 마우스 Y: Z축 이동, X: X축 이동
            model->position.setX(model->position.x() + delta.x() * sensitivity);
            model->position.setZ(model->position.z() - delta.y() * sensitivity);
            emit m_scene->modelTransformChanged(m_selectedModelIdx);
        }
        else if (m_transformMode == TransformMode::Rotate && e->buttons() & Qt::LeftButton) {
            // 회전: X축 회전(Y delta), Y축 회전(X delta)
            model->rotation.setX(model->rotation.x() + delta.y() * 0.5f);
            model->rotation.setY(model->rotation.y() + delta.x() * 0.5f);
            emit m_scene->modelTransformChanged(m_selectedModelIdx);
        }
        else if (m_transformMode == TransformMode::Scale && e->buttons() & Qt::LeftButton) {
            // 스케일: Y delta로 균일 스케일 조정
            float scaleChange = 1.0f + delta.y() * 0.002f;
            model->scale *= scaleChange;
            emit m_scene->modelTransformChanged(m_selectedModelIdx);
        }
        update();
        return;
    }

    // 일반 카메라 컨트롤
    if (m_activeButton == Qt::LeftButton) {
        m_camera.orbit(delta.x() * 0.4f, delta.y() * 0.4f);
    } else if (m_activeButton == Qt::MiddleButton ||
               (e->buttons() & Qt::RightButton && e->modifiers() & Qt::ShiftModifier)) {
        m_camera.pan(delta.x(), delta.y());
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent *) { m_activeButton = Qt::NoButton; }

int Viewport::pickModel(int x, int y)
{
    // 간단한 AABB 기반 피킹
    // 더 정확한 피킹은 GPU 기반 선택 또는 메시 기반 레이캐스트 필요
    
    // 뷰포트 좌표를 NDC로 변환
    float ndcX = (2.0f * x) / width() - 1.0f;
    float ndcY = 1.0f - (2.0f * y) / height();
    
    QMatrix4x4 proj = m_camera.projectionMatrix(float(width()) / float(height()));
    QMatrix4x4 view = m_camera.viewMatrix();
    QMatrix4x4 invProj = proj.inverted();
    QMatrix4x4 invView = view.inverted();
    
    // Ray 생성 (near plane에서 far plane으로)
    QVector4D rayNear = invProj * QVector4D(ndcX, ndcY, -1.0f, 1.0f);
    rayNear /= rayNear.w();
    
    QVector4D rayFar = invProj * QVector4D(ndcX, ndcY, 1.0f, 1.0f);
    rayFar /= rayFar.w();
    
    QVector3D rayOrigin = (invView * rayNear).toVector3D();
    QVector3D rayDir = ((invView * rayFar).toVector3D() - rayOrigin).normalized();
    
    // 간단한 구 기반 교차 검사
    int closestIdx = -1;
    float minDist = FLT_MAX;
    
    const auto &models = m_scene->models();
    for (size_t i = 0; i < models.size(); ++i) {
        const auto &model = models[i];
        
        // 구의 반지름을 대략적으로 계산 (실제로는 AABB 또는 바운딩 구 사용)
        float sphereRadius = 1.5f * (model->scale.x() + model->scale.y() + model->scale.z()) / 3.0f;
        
        // Ray-sphere intersection
        QVector3D oc = rayOrigin - model->position;
        float a = QVector3D::dotProduct(rayDir, rayDir);
        float b = 2.0f * QVector3D::dotProduct(oc, rayDir);
        float c = QVector3D::dotProduct(oc, oc) - sphereRadius * sphereRadius;
        float discriminant = b * b - 4.0f * a * c;
        
        if (discriminant >= 0.0f) {
            float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
            if (t > 0.0f && t < minDist) {
                minDist = t;
                closestIdx = i;
            }
        }
    }
    
    return closestIdx;
}

void Viewport::wheelEvent(QWheelEvent *e)
{
    float d = e->angleDelta().y() / 120.f;
    m_camera.zoom(d);
}

void Viewport::keyPressEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat()) {
        QOpenGLWidget::keyPressEvent(e);
        return;
    }
    
    switch (e->key()) {
    case Qt::Key_R:
        // R: 기본 카메라 리셋이거나 회전 모드
        if (m_selectedModelIdx >= 0 && m_transformMode == TransformMode::None) {
            m_transformMode = TransformMode::Rotate;
        } else {
            resetCamera();
        }
        break;
        
    case Qt::Key_G:  
        // G: Move 모드
        if (m_selectedModelIdx >= 0) {
            m_transformMode = (m_transformMode == TransformMode::Move) 
                            ? TransformMode::None 
                            : TransformMode::Move;
        }
        break;
        
    case Qt::Key_S:
        // S: Scale 모드
        if (m_selectedModelIdx >= 0) {
            m_transformMode = (m_transformMode == TransformMode::Scale) 
                            ? TransformMode::None 
                            : TransformMode::Scale;
        }
        break;
        
    case Qt::Key_Escape:
        // Esc: 현재 모드 취소
        m_transformMode = TransformMode::None;
        break;
        
    case Qt::Key_F:  
        onModelAdded();
        break;
        
    case Qt::Key_Space: 
        toggleAnimation();
        break;
        
    default: 
        QOpenGLWidget::keyPressEvent(e);
    }
    
    update();
}