#include "AnimationPlayer.h"

AnimationPlayer::AnimationPlayer(QObject *parent) : QObject(parent)
{
    m_timer.start();
    m_lastMs = m_timer.elapsed();
}

void AnimationPlayer::play()
{
    if (m_playing) return;
    m_playing = true;
    m_lastMs  = m_timer.elapsed();
    emit playStateChanged(true);
}

void AnimationPlayer::pause()
{
    if (!m_playing) return;
    m_playing = false;
    emit playStateChanged(false);
}

void AnimationPlayer::stop()
{
    m_playing     = false;
    m_currentTime = 0.0;
    emit timeChanged(0.0);
    emit playStateChanged(false);
}

void AnimationPlayer::tick()
{
    if (!m_playing) return;
    qint64 now  = m_timer.elapsed();
    double dt   = (now - m_lastMs) / 1000.0 * m_speed;
    m_lastMs    = now;
    m_currentTime += dt;
    if (m_duration > 0.0 && m_currentTime > m_duration)
        m_currentTime = std::fmod(m_currentTime, m_duration);
    emit timeChanged(m_currentTime);
}
