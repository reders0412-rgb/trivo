#pragma once
#include <QWidget>
#include <memory>

class QLabel;
struct SceneModel;

class InfoPanel : public QWidget
{
    Q_OBJECT
public:
    explicit InfoPanel(QWidget *parent = nullptr);

public slots:
    void refresh(std::shared_ptr<SceneModel> model = nullptr);
    void clear();

private:
    QLabel *m_label = nullptr;
};
