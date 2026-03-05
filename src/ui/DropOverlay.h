#pragma once
#include <QWidget>

class DropOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit DropOverlay(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
};
