#pragma once
#include <QObject>
#include <QElapsedTimer>

class AnimationPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AnimationPlayer(QObject *parent = nullptr);

    void   play();
    void   pause();
    void   stop();
    void   setSpeed(double s) { m_speed = s; }
    void   setDuration(double secs) { m_duration = secs; }

    bool   isPlaying() const { return m_playing; }
    double currentTime() const { return m_currentTime; }
    double speed()       const { return m_speed; }

    void   tick(); // called each frame

signals:
    void timeChanged(double t);
    void playStateChanged(bool playing);

private:
    bool           m_playing     = false;
    double         m_currentTime = 0.0;
    double         m_speed       = 1.0;
    double         m_duration    = 0.0;
    QElapsedTimer  m_timer;
    qint64         m_lastMs      = 0;
};
