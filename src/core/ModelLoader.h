#pragma once
#include "Scene.h"
#include <QString>
#include <memory>

class ModelLoader
{
public:
    // Loads a 3D file using Assimp and returns a populated SceneModel.
    // Returns nullptr on failure.
    static std::shared_ptr<SceneModel> load(const QString &path);

    // Human-readable error from last load attempt
    static QString lastError();

private:
    static QString s_lastError;
};
