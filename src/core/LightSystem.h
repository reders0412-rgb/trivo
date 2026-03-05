#pragma once
#include <QVector3D>
#include <QColor>
#include <vector>
#include <QString>

struct Light {
    enum Type { Directional, Point, Spot } type = Directional;
    QVector3D direction{-1,-1,-1};
    QVector3D position{0,5,0};
    QColor    color{255,255,255};
    float     intensity = 1.f;
    bool      enabled   = true;
};

struct LightPreset {
    QString name;
    std::vector<Light> lights;
};

class LightSystem
{
public:
    LightSystem();

    static std::vector<LightPreset> presets();
    void applyPreset(int index);

    const std::vector<Light> &lights() const { return m_lights; }
    std::vector<Light>       &lights()       { return m_lights; }

    QColor ambientColor()    const { return m_ambient; }
    float  ambientStrength() const { return m_ambientStr; }
    void   setAmbient(const QColor &c, float s) { m_ambient = c; m_ambientStr = s; }

private:
    std::vector<Light> m_lights;
    QColor m_ambient{30,30,40};
    float  m_ambientStr = 0.15f;
};
