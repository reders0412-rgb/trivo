#include "InfoPanel.h"
#include "../core/Scene.h"
#include "../utils/FileUtils.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>

InfoPanel::InfoPanel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 2, 8, 2);
    layout->setSpacing(2);
    
    m_label = new QLabel(this);
    m_label->setStyleSheet("font-size: 11px;");
    m_label->setWordWrap(true);
    layout->addWidget(m_label);
    layout->addStretch();
    
    clear();
}

void InfoPanel::clear()
{
    m_label->setText("Trivo  —  파일을 열거나 드래그하세요  /  Open or drag a 3D file here");
}

void InfoPanel::refresh(std::shared_ptr<SceneModel> model)
{
    if (!model) { clear(); return; }

    QFileInfo fi(model->filePath);
    const QString size = FileUtils::humanSize(fi.size());
    const QString fmt  = FileUtils::formatName(fi.suffix().toLower());
    const QString anim = model->hasAnim ? "  🎬" : "";
    
    // Transform 정보 추가
    QString transformInfo = QString(
        "📄 %1  │  %2  │  %3  │  버텍스 %4  삼각형 %5  메시 %6  재질 %7  본 %8%9\n"
        "   위치(Position): [%.2f, %.2f, %.2f]  │  "
        "회전(Rotation): [%.2f°, %.2f°, %.2f°]  │  "
        "스케일(Scale): [%.2f, %.2f, %.2f]")
        .arg(model->name)
        .arg(fmt)
        .arg(size)
        .arg(model->vertCount)
        .arg(model->triCount)
        .arg(model->meshCount)
        .arg(model->matCount)
        .arg(model->boneCount)
        .arg(anim)
        .arg(model->position.x())
        .arg(model->position.y())
        .arg(model->position.z())
        .arg(model->rotation.x())
        .arg(model->rotation.y())
        .arg(model->rotation.z())
        .arg(model->scale.x())
        .arg(model->scale.y())
        .arg(model->scale.z());

    m_label->setText(transformInfo);
}
