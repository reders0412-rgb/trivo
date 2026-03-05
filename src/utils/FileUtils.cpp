#include "FileUtils.h"
#include <QFileInfo>
#include <QMap>

QStringList FileUtils::supportedExtensions()
{
    return {
        // Modern / game
        "glb", "gltf",
        "fbx",
        // Common interchange
        "obj",
        "dae",      // COLLADA
        "3ds",      // 3ds Max legacy
        "blend",    // Blender (via Assimp)
        // CAD / Engineering
        "stl",      // Stereolithography
        "ply",      // Stanford Polygon
        "step","stp","iges","igs",
        // Point cloud / scan
        "pcd", "xyz", "pts",
        // Animation / game
        "bvh",      // BioVision Hierarchy
        "md2","md3","md5mesh",
        "x",        // DirectX
        "lwo","lws",// LightWave
        "ms3d",     // MilkShape
        "cob",      // Caligari
        "dxf",
        "ifc",
        "q3o","q3s",
        "ter",      // Terragen
        "raw",
        "ac","ac3d",
        "nff","enff",
        "off",
        "smd","vta",// Valve
        "mdc",
        "m3",
        "pk3",
        "assbin",
    };
}

static const QMap<QString, QString> &fmtMap()
{
    static QMap<QString, QString> m {
        {"glb","GL Transmission Format (Binary)"},
        {"gltf","GL Transmission Format"},
        {"fbx","Filmbox"},
        {"obj","Wavefront OBJ"},
        {"dae","COLLADA"},
        {"3ds","3ds Max"},
        {"blend","Blender"},
        {"stl","Stereolithography"},
        {"ply","Stanford Polygon"},
        {"step","STEP / ISO 10303"},
        {"stp","STEP / ISO 10303"},
        {"iges","IGES"},
        {"igs","IGES"},
        {"bvh","BioVision Hierarchy"},
        {"x","DirectX Mesh"},
        {"lwo","LightWave Object"},
        {"dxf","Drawing Exchange Format"},
        {"ifc","Industry Foundation Classes"},
        {"pcd","Point Cloud Data"},
    };
    return m;
}

QString FileUtils::formatName(const QString &ext)
{
    return fmtMap().value(ext.toLower(), ext.toUpper() + " File");
}

bool FileUtils::isSupported(const QString &path)
{
    const QString ext = QFileInfo(path).suffix().toLower();
    return supportedExtensions().contains(ext);
}

QString FileUtils::filterString()
{
    QStringList exts;
    for (const auto &e : supportedExtensions())
        exts << "*." + e;

    QString all = "3D Files (" + exts.join(" ") + ")";

    // Individual groups
    QString groups;
    groups += ";;GL Transmission (*.glb *.gltf)";
    groups += ";;Filmbox (*.fbx)";
    groups += ";;Wavefront OBJ (*.obj)";
    groups += ";;COLLADA (*.dae)";
    groups += ";;3ds Max (*.3ds)";
    groups += ";;Stereolithography (*.stl)";
    groups += ";;Stanford PLY (*.ply)";
    groups += ";;All Files (*)";

    return all + groups;
}

QString FileUtils::humanSize(qint64 bytes)
{
    if (bytes < 1024)        return QString::number(bytes) + " B";
    if (bytes < 1024*1024)   return QString::number(bytes/1024.0, 'f', 1) + " KB";
    if (bytes < 1024*1024*1024) return QString::number(bytes/1024.0/1024.0, 'f', 1) + " MB";
    return QString::number(bytes/1024.0/1024.0/1024.0, 'f', 2) + " GB";
}
