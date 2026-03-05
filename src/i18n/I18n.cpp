#include "I18n.h"
#include <QSettings>

I18n &I18n::instance()
{
    static I18n s;
    return s;
}

void I18n::init()
{
    loadStrings();
    QSettings s;
    m_lang = s.value("language", "ko").toString();
}

void I18n::setLanguage(const QString &lang)
{
    if (m_lang == lang) return;
    m_lang = lang;
    QSettings s;
    s.setValue("language", lang);
    emit languageChanged(lang);
}

QString I18n::tr(const QString &key) const
{
    auto it = m_strings.find(m_lang);
    if (it == m_strings.end()) it = m_strings.find("en");
    if (it == m_strings.end()) return key;
    return it->value(key, key);
}

void I18n::loadStrings()
{
    // ── Korean ────────────────────────────────────────────────────────────────
    auto &ko = m_strings["ko"];
    ko["app.name"]           = "Trivo";
    ko["file.open"]          = "열기…";
    ko["file.add"]           = "씬에 추가…";
    ko["file.clear"]         = "씬 비우기";
    ko["file.screenshot"]    = "스크린샷 저장";
    ko["camera.reset"]       = "카메라 초기화";
    ko["anim.play"]          = "재생";
    ko["anim.pause"]         = "일시 정지";
    ko["anim.stop"]          = "정지";
    ko["anim.speed"]         = "속도";
    ko["theme.dark"]         = "다크 모드";
    ko["theme.light"]        = "라이트 모드";
    ko["lang.ko"]            = "한국어";
    ko["lang.en"]            = "English";
    ko["info.vertices"]      = "버텍스";
    ko["info.triangles"]     = "삼각형";
    ko["info.meshes"]        = "메시";
    ko["info.materials"]     = "재질";
    ko["info.bones"]         = "본";
    ko["info.animations"]    = "애니메이션";
    ko["info.file"]          = "파일";
    ko["info.format"]        = "형식";
    ko["info.size"]          = "파일 크기";
    ko["scene.models"]       = "씬의 모델";
    ko["scene.background"]   = "배경색";
    ko["scene.lights"]       = "라이트 프리셋";
    ko["scene.add"]          = "모델 추가";
    ko["scene.remove"]       = "모델 제거";
    ko["about.title"]        = "Trivo 정보";
    ko["about.desc"]         = "Trivo — 오픈소스 3D 파일 뷰어";
    ko["about.version"]      = "버전";
    ko["about.libraries"]    = "사용 라이브러리 및 라이선스";
    ko["drop.hint"]          = "3D 파일을 여기에 드래그 앤 드롭";
    ko["drop.hint2"]         = "GLB · FBX · OBJ · GLTF · STL · DAE · 3DS · 더 보기…";
    ko["panel.transform"]    = "변환";
    ko["panel.pos"]          = "위치";
    ko["panel.rot"]          = "회전";
    ko["panel.scale"]        = "크기";
    ko["panel.visible"]      = "표시";
    ko["toolbar.open"]       = "열기";
    ko["toolbar.add"]        = "추가";
    ko["toolbar.reset"]      = "초기화";
    ko["toolbar.play"]       = "재생";
    ko["toolbar.screenshot"] = "캡처";

    // ── English ───────────────────────────────────────────────────────────────
    auto &en = m_strings["en"];
    en["app.name"]           = "Trivo";
    en["file.open"]          = "Open…";
    en["file.add"]           = "Add to Scene…";
    en["file.clear"]         = "Clear Scene";
    en["file.screenshot"]    = "Save Screenshot";
    en["camera.reset"]       = "Reset Camera";
    en["anim.play"]          = "Play";
    en["anim.pause"]         = "Pause";
    en["anim.stop"]          = "Stop";
    en["anim.speed"]         = "Speed";
    en["theme.dark"]         = "Dark Mode";
    en["theme.light"]        = "Light Mode";
    en["lang.ko"]            = "한국어";
    en["lang.en"]            = "English";
    en["info.vertices"]      = "Vertices";
    en["info.triangles"]     = "Triangles";
    en["info.meshes"]        = "Meshes";
    en["info.materials"]     = "Materials";
    en["info.bones"]         = "Bones";
    en["info.animations"]    = "Animations";
    en["info.file"]          = "File";
    en["info.format"]        = "Format";
    en["info.size"]          = "File Size";
    en["scene.models"]       = "Scene Models";
    en["scene.background"]   = "Background Color";
    en["scene.lights"]       = "Light Preset";
    en["scene.add"]          = "Add Model";
    en["scene.remove"]       = "Remove Model";
    en["about.title"]        = "About Trivo";
    en["about.desc"]         = "Trivo — Open source 3D file viewer";
    en["about.version"]      = "Version";
    en["about.libraries"]    = "Libraries & Licenses";
    en["drop.hint"]          = "Drag & drop a 3D file here";
    en["drop.hint2"]         = "GLB · FBX · OBJ · GLTF · STL · DAE · 3DS · and more…";
    en["panel.transform"]    = "Transform";
    en["panel.pos"]          = "Position";
    en["panel.rot"]          = "Rotation";
    en["panel.scale"]        = "Scale";
    en["panel.visible"]      = "Visible";
    en["toolbar.open"]       = "Open";
    en["toolbar.add"]        = "Add";
    en["toolbar.reset"]      = "Reset";
    en["toolbar.play"]       = "Play";
    en["toolbar.screenshot"] = "Capture";
}
