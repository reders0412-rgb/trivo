#include "IconProvider.h"
#include <QFileInfo>
#include <QMap>

QIcon IconProvider::forExtensionPath(const QString &filePath)
{
    return forExtension(QFileInfo(filePath).suffix().toLower());
}

QIcon IconProvider::forExtension(const QString &ext)
{
    static const QMap<QString, QString> map {
        {"glb",  ":/icons/ext_glb.svg"},
        {"gltf", ":/icons/ext_gltf.svg"},
        {"fbx",  ":/icons/ext_fbx.svg"},
        {"obj",  ":/icons/ext_obj.svg"},
        {"dae",  ":/icons/ext_dae.svg"},
        {"3ds",  ":/icons/ext_3ds.svg"},
        {"stl",  ":/icons/ext_stl.svg"},
        {"ply",  ":/icons/ext_ply.svg"},
        {"blend",":/icons/ext_blend.svg"},
        {"bvh",  ":/icons/ext_bvh.svg"},
        {"x",    ":/icons/ext_x.svg"},
        {"dxf",  ":/icons/ext_dxf.svg"},
        {"ifc",  ":/icons/ext_ifc.svg"},
        {"step", ":/icons/ext_step.svg"},
        {"stp",  ":/icons/ext_step.svg"},
        {"iges", ":/icons/ext_iges.svg"},
        {"igs",  ":/icons/ext_iges.svg"},
    };

    const QString path = map.value(ext, ":/icons/ext_generic.svg");
    return QIcon(path);
}

QIcon IconProvider::appIcon()        { return QIcon(":/icons/trivo_app.svg"); }
QIcon IconProvider::openIcon()       { return QIcon(":/icons/open.svg"); }
QIcon IconProvider::addIcon()        { return QIcon(":/icons/add.svg"); }
QIcon IconProvider::resetIcon()      { return QIcon(":/icons/reset.svg"); }
QIcon IconProvider::playIcon()       { return QIcon(":/icons/play.svg"); }
QIcon IconProvider::pauseIcon()      { return QIcon(":/icons/pause.svg"); }
QIcon IconProvider::screenshotIcon() { return QIcon(":/icons/camera.svg"); }
QIcon IconProvider::darkThemeIcon()  { return QIcon(":/icons/moon.svg"); }
QIcon IconProvider::lightThemeIcon() { return QIcon(":/icons/sun.svg"); }
