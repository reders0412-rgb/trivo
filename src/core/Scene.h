#pragma once
#include <QObject>
#include <QVector3D>
#include <QString>
#include <QColor>
#include <vector>
#include <memory>

struct SceneModel {
    QString  filePath;
    QString  name;
    QVector3D position{0,0,0};
    QVector3D rotation{0,0,0};
    QVector3D scale{1,1,1};
    bool     visible   = true;
    bool     hasAnim   = false;
    int      vertCount = 0;
    int      triCount  = 0;
    int      meshCount = 0;
    int      matCount  = 0;
    int      boneCount = 0;
    // GPU handle filled by Renderer
    unsigned int vao = 0;
    // raw assimp scene (managed by ModelLoader)
    void *assimpScene = nullptr;
};

class Scene : public QObject
{
    Q_OBJECT

public:
    explicit Scene(QObject *parent = nullptr);

    void addModel(const QString &path);
    void clear();
    void removeModel(int index);

    const std::vector<std::shared_ptr<SceneModel>> &models() const { return m_models; }
    
    // 모델 선택/변환 기능
    int getSelectedModelIndex() const { return m_selectedModelIndex; }
    void setSelectedModelIndex(int idx) { m_selectedModelIndex = idx; emit selectedModelChanged(idx); }
    std::shared_ptr<SceneModel> getSelectedModel() const;
    
    void updateModelTransform(int idx, const QVector3D &pos, const QVector3D &rot, const QVector3D &scale);

    QColor backgroundColor() const { return m_bgColor; }
    void   setBackgroundColor(const QColor &c) { m_bgColor = c; emit backgroundColorChanged(c); }

signals:
    void modelAdded(std::shared_ptr<SceneModel>);
    void modelRemoved(int index);
    void sceneCleared();
    void backgroundColorChanged(const QColor &c);
    void selectedModelChanged(int index);
    void modelTransformChanged(int index);

private:
    std::vector<std::shared_ptr<SceneModel>> m_models;
    QColor m_bgColor{30, 30, 38};
    int m_selectedModelIndex = -1;
};
