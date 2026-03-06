#include "Scene.h"
#include "ModelLoader.h"
#include <QFileInfo>
#include <QVector3D>

Scene::Scene(QObject *parent) : QObject(parent) {}

void Scene::addModel(const QString &path)
{
    auto model = ModelLoader::load(path);
    if (!model) return;

    // Offset each added model slightly so they don't overlap
    float offset = static_cast<float>(m_models.size()) * 1.5f;
    model->position = QVector3D(offset, 0.f, 0.f);

    m_models.push_back(model);
    emit modelAdded(model);
}

void Scene::clear()
{
    m_models.clear();
    m_selectedModelIndex = -1;
    emit sceneCleared();
}

void Scene::removeModel(int index)
{
    if (index < 0 || index >= static_cast<int>(m_models.size())) return;
    m_models.erase(m_models.begin() + index);
    
    // 선택된 모델이 제거되면 선택 해제
    if (m_selectedModelIndex == index) {
        m_selectedModelIndex = -1;
    } else if (m_selectedModelIndex > index) {
        m_selectedModelIndex--;
    }
    
    emit modelRemoved(index);
}

std::shared_ptr<SceneModel> Scene::getSelectedModel() const
{
    if (m_selectedModelIndex < 0 || m_selectedModelIndex >= static_cast<int>(m_models.size())) {
        return nullptr;
    }
    return m_models[m_selectedModelIndex];
}

void Scene::updateModelTransform(int idx, const QVector3D &pos, const QVector3D &rot, const QVector3D &scale)
{
    if (idx < 0 || idx >= static_cast<int>(m_models.size())) return;
    
    auto model = m_models[idx];
    model->position = pos;
    model->rotation = rot;
    model->scale = scale;
    
    emit modelTransformChanged(idx);
}
