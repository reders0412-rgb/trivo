#include "LightSystem.h"

LightSystem::LightSystem() { applyPreset(0); }

std::vector<LightPreset> LightSystem::presets()
{
    // ── Studio ────────────────────────────────────────────────────────────────
    LightPreset studio;
    studio.name = "Studio";
    {
        Light key; key.direction = {-1,-1.5f,-1}; key.intensity = 1.2f; key.color = {255,248,230};
        Light fill; fill.direction = {1,-0.5f,-1}; fill.intensity = 0.6f; fill.color = {200,220,255};
        Light rim;  rim.direction  = {0.5f,1,1};   rim.intensity  = 0.4f; rim.color  = {255,255,255};
        studio.lights = {key, fill, rim};
    }

    // ── Outdoor ───────────────────────────────────────────────────────────────
    LightPreset outdoor;
    outdoor.name = "Outdoor";
    {
        Light sun; sun.direction = {-0.5f,-1,-0.5f}; sun.intensity = 1.5f; sun.color = {255,244,200};
        Light sky; sky.direction = {0,-1,0};          sky.intensity = 0.3f; sky.color = {120,180,255};
        outdoor.lights = {sun, sky};
    }

    // ── Dramatic ──────────────────────────────────────────────────────────────
    LightPreset dramatic;
    dramatic.name = "Dramatic";
    {
        Light main; main.direction = {-1,-1,0}; main.intensity = 2.f;  main.color = {255,200,150};
        Light back; back.direction = {1,0.5f,1}; back.intensity = 0.2f; back.color = {100,100,200};
        dramatic.lights = {main, back};
    }

    // ── Soft ──────────────────────────────────────────────────────────────────
    LightPreset soft;
    soft.name = "Soft";
    {
        auto makeDir = [](float x, float y, float z, float i, int r, int g, int b) {
            Light l; l.direction = {x,y,z}; l.intensity = i; l.color = {r,g,b}; return l;
        };
        soft.lights = {
            makeDir(-1,-1,-1, 0.8f, 255,250,245),
            makeDir( 1,-1,-1, 0.8f, 245,250,255),
            makeDir( 0, 1, 0, 0.4f, 255,255,255),
            makeDir( 0,-1, 1, 0.5f, 240,245,255),
        };
    }

    // ── Night ─────────────────────────────────────────────────────────────────
    LightPreset night;
    night.name = "Night";
    {
        Light moon; moon.direction = {-0.3f,-1,-0.5f}; moon.intensity = 0.4f; moon.color = {180,200,255};
        Light lamp; lamp.type = Light::Point; lamp.position = {2,3,0}; lamp.intensity = 2.f; lamp.color = {255,200,100};
        night.lights = {moon, lamp};
    }

    return {studio, outdoor, dramatic, soft, night};
}

void LightSystem::applyPreset(int index)
{
    auto ps = presets();
    if (index >= 0 && index < static_cast<int>(ps.size()))
        m_lights = ps[index].lights;
}
