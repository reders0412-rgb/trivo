#include "Camera.h"
#include <QtMath>
#include <algorithm>

Camera::Camera() { updatePosition(); }

void Camera::orbit(float dYaw, float dPitch)
{
    m_yaw   += dYaw;
    m_pitch  = std::clamp(m_pitch + dPitch, -89.f, 89.f);
    updatePosition();
}

void Camera::pan(float dx, float dy)
{
    QVector3D forward = (m_target - m_position).normalized();
    QVector3D right   = QVector3D::crossProduct(forward, {0,1,0}).normalized();
    QVector3D up      = QVector3D::crossProduct(right, forward).normalized();

    float s = m_distance * 0.001f;
    m_target += -right * dx * s + up * dy * s;
    updatePosition();
}

void Camera::zoom(float delta)
{
    m_distance = std::max(0.01f, m_distance - delta * m_distance * 0.1f);
    updatePosition();
}

void Camera::fitToScene(const QVector3D &center, float radius)
{
    m_target   = center;
    m_distance = radius / std::tan(qDegreesToRadians(m_fov * 0.5f)) * 1.5f;
    m_near     = m_distance * 0.001f;
    m_far      = m_distance * 100.f;
    updatePosition();
}

void Camera::reset()
{
    m_target   = {0,0,0};
    m_distance = 5.f;
    m_yaw      = 45.f;
    m_pitch    = 20.f;
    updatePosition();
}

QMatrix4x4 Camera::viewMatrix() const
{
    QMatrix4x4 m;
    m.lookAt(m_position, m_target, {0,1,0});
    return m;
}

QMatrix4x4 Camera::projectionMatrix(float aspect) const
{
    QMatrix4x4 m;
    m.perspective(m_fov, aspect, m_near, m_far);
    return m;
}

void Camera::updatePosition()
{
    float yaw   = qDegreesToRadians(m_yaw);
    float pitch = qDegreesToRadians(m_pitch);
    m_position  = m_target + QVector3D(
        m_distance * std::cos(pitch) * std::sin(yaw),
        m_distance * std::sin(pitch),
        m_distance * std::cos(pitch) * std::cos(yaw)
    );
}
