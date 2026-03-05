#pragma once
#include <QVector3D>
#include <QMatrix4x4>

class Camera
{
public:
    Camera();

    // Orbit controls
    void orbit(float dYaw, float dPitch);
    void pan(float dx, float dy);
    void zoom(float delta);

    // Fit all models in view
    void fitToScene(const QVector3D &center, float radius);
    void reset();

    QMatrix4x4 viewMatrix()       const;
    QMatrix4x4 projectionMatrix(float aspect) const;
    QVector3D  position()         const { return m_position; }
    QVector3D  target()           const { return m_target; }

    float fov()  const { return m_fov; }
    float near() const { return m_near; }
    float far()  const { return m_far; }

private:
    void updatePosition();

    QVector3D m_target{0,0,0};
    float m_distance = 5.f;
    float m_yaw      = 45.f;
    float m_pitch    = 20.f;
    float m_fov      = 45.f;
    float m_near     = 0.01f;
    float m_far      = 10000.f;
    QVector3D m_position;
};
