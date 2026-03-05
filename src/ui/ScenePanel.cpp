#include "ScenePanel.h"
#include "../core/Scene.h"
#include "../utils/IconProvider.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QFileInfo>
#include <QHeaderView>

ScenePanel::ScenePanel(std::shared_ptr<Scene> scene, QWidget *parent)
    : QWidget(parent), m_scene(scene)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    // Tree
    m_tree = new QTreeWidget(this);
    m_tree->setHeaderHidden(true);
    m_tree->setRootIsDecorated(false);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setMinimumHeight(120);
    layout->addWidget(m_tree);

    // Remove button
    auto *removeBtn = new QPushButton("모델 제거 / Remove", this);
    layout->addWidget(removeBtn);
    connect(removeBtn, &QPushButton::clicked, this, &ScenePanel::onRemoveSelected);

    // Transform group
    m_xformGroup = new QGroupBox("변환 / Transform", this);
    auto *xformLayout = new QVBoxLayout(m_xformGroup);
    xformLayout->setSpacing(4);

    auto makeRow = [&](const QString &label, QDoubleSpinBox **spins) {
        auto *row = new QHBoxLayout;
        auto *lbl = new QLabel(label, m_xformGroup);
        lbl->setFixedWidth(50);
        row->addWidget(lbl);
        for (int i=0; i<3; ++i) {
            spins[i] = new QDoubleSpinBox(m_xformGroup);
            spins[i]->setRange(-99999, 99999);
            spins[i]->setSingleStep(0.1);
            spins[i]->setDecimals(3);
            row->addWidget(spins[i]);
            connect(spins[i], QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    this, &ScenePanel::onTransformChanged);
        }
        xformLayout->addLayout(row);
    };
    makeRow("위치/Pos", m_pos);
    makeRow("회전/Rot", m_rot);
    makeRow("크기/Scl", m_scl);

    // Visible checkbox
    m_visible = new QCheckBox("표시 / Visible", m_xformGroup);
    m_visible->setChecked(true);
    xformLayout->addWidget(m_visible);
    connect(m_visible, &QCheckBox::toggled, this, &ScenePanel::onTransformChanged);

    m_xformGroup->setEnabled(false);
    layout->addWidget(m_xformGroup);
    layout->addStretch();

    connect(m_tree, &QTreeWidget::itemClicked, this, &ScenePanel::onModelSelected);
}

void ScenePanel::refresh()
{
    populateTree();
}

void ScenePanel::populateTree()
{
    m_tree->clear();
    const auto &models = m_scene->models();
    for (int i=0; i<(int)models.size(); ++i) {
        const auto &m = models[i];
        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, m->name);
        item->setIcon(0, IconProvider::forExtensionPath(m->filePath));
        item->setData(0, Qt::UserRole, i);
    }
}

void ScenePanel::onModelSelected(QTreeWidgetItem *item, int)
{
    if (!item) return;
    selectModel(item->data(0, Qt::UserRole).toInt());
}

void ScenePanel::selectModel(int index)
{
    const auto &models = m_scene->models();
    if (index < 0 || index >= (int)models.size()) {
        m_xformGroup->setEnabled(false);
        m_selected = -1;
        return;
    }
    m_selected = index;
    m_xformGroup->setEnabled(true);

    m_updating = true;
    const auto &mdl = *models[index];
    m_pos[0]->setValue(mdl.position.x());
    m_pos[1]->setValue(mdl.position.y());
    m_pos[2]->setValue(mdl.position.z());
    m_rot[0]->setValue(mdl.rotation.x());
    m_rot[1]->setValue(mdl.rotation.y());
    m_rot[2]->setValue(mdl.rotation.z());
    m_scl[0]->setValue(mdl.scale.x());
    m_scl[1]->setValue(mdl.scale.y());
    m_scl[2]->setValue(mdl.scale.z());
    m_visible->setChecked(mdl.visible);
    m_updating = false;
}

void ScenePanel::onTransformChanged()
{
    if (m_updating || m_selected < 0) return;
    const auto &models = m_scene->models();
    if (m_selected >= (int)models.size()) return;
    auto &mdl = *models[m_selected];
    mdl.position = {(float)m_pos[0]->value(),(float)m_pos[1]->value(),(float)m_pos[2]->value()};
    mdl.rotation = {(float)m_rot[0]->value(),(float)m_rot[1]->value(),(float)m_rot[2]->value()};
    mdl.scale    = {(float)m_scl[0]->value(),(float)m_scl[1]->value(),(float)m_scl[2]->value()};
    mdl.visible  = m_visible->isChecked();
}

void ScenePanel::onRemoveSelected()
{
    if (m_selected < 0) return;
    m_scene->removeModel(m_selected);
    m_selected = -1;
    m_xformGroup->setEnabled(false);
    populateTree();
}
