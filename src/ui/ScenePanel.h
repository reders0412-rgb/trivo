#pragma once
#include <QWidget>
#include <memory>

class Scene;
class QTreeWidget;
class QTreeWidgetItem;
class QDoubleSpinBox;
class QCheckBox;
class QGroupBox;

class ScenePanel : public QWidget
{
    Q_OBJECT
public:
    explicit ScenePanel(std::shared_ptr<Scene> scene, QWidget *parent = nullptr);
    void refresh();

private slots:
    void onModelSelected(QTreeWidgetItem *item, int col);
    void onRemoveSelected();
    void onTransformChanged();

private:
    void buildTransformUI(QGroupBox *group);
    void populateTree();
    void selectModel(int index);

    std::shared_ptr<Scene> m_scene;
    QTreeWidget  *m_tree    = nullptr;
    QGroupBox    *m_xformGroup = nullptr;
    QDoubleSpinBox *m_pos[3] = {}, *m_rot[3] = {}, *m_scl[3] = {};
    QCheckBox    *m_visible  = nullptr;
    int           m_selected = -1;
    bool          m_updating = false;
};
