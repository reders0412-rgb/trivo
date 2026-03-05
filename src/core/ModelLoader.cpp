#include "ModelLoader.h"
#include <QFileInfo>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

QString ModelLoader::s_lastError;

// ── Helpers ───────────────────────────────────────────────────────────────────

static void countMeshData(const aiScene *sc, int &verts, int &tris, int &bones)
{
    verts = tris = bones = 0;
    for (unsigned i = 0; i < sc->mNumMeshes; ++i) {
        const aiMesh *m = sc->mMeshes[i];
        verts += static_cast<int>(m->mNumVertices);
        tris  += static_cast<int>(m->mNumFaces);
        bones += static_cast<int>(m->mNumBones);
    }
}

// ── Public ────────────────────────────────────────────────────────────────────

std::shared_ptr<SceneModel> ModelLoader::load(const QString &path)
{
    // Use a fresh Importer per call (thread-safe)
    Assimp::Importer importer;

    const unsigned flags =
        aiProcess_Triangulate           |
        aiProcess_GenSmoothNormals      |
        aiProcess_CalcTangentSpace      |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs               |
        aiProcess_ValidateDataStructure |
        aiProcess_ImproveCacheLocality  |
        aiProcess_OptimizeMeshes;

    const aiScene *scene = importer.ReadFile(path.toStdString(), flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        s_lastError = QString::fromStdString(importer.GetErrorString());
        return nullptr;
    }

    auto model = std::make_shared<SceneModel>();
    model->filePath  = path;
    model->name      = QFileInfo(path).fileName();
    model->meshCount = static_cast<int>(scene->mNumMeshes);
    model->matCount  = static_cast<int>(scene->mNumMaterials);
    model->hasAnim   = scene->HasAnimations();

    int verts, tris, bones;
    countMeshData(scene, verts, tris, bones);
    model->vertCount  = verts;
    model->triCount   = tris;
    model->boneCount  = bones;

    // Keep the raw pointer alive through the model lifetime.
    // The Importer owns the scene but we transfer ownership here.
    // We store it as void* to avoid exposing Assimp headers in the header.
    // The Renderer will cast it back to const aiScene*.
    // NOTE: We detach the scene from the importer so it is not freed.
    model->assimpScene = const_cast<aiScene*>(importer.GetOrphanedScene());

    return model;
}

QString ModelLoader::lastError()
{
    return s_lastError;
}
