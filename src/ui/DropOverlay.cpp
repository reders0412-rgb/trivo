#include "DropOverlay.h"
#include <QPainter>
#include <QDragLeaveEvent>

DropOverlay::DropOverlay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

void DropOverlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Semi-transparent overlay
    p.fillRect(rect(), QColor(30, 100, 220, 80));

    // Rounded border
    QPen pen(QColor(80, 160, 255), 3, Qt::DashLine);
    p.setPen(pen);
    p.drawRoundedRect(rect().adjusted(20,20,-20,-20), 16, 16);

    // Text
    p.setPen(Qt::white);
    QFont f;
    f.setPointSize(20);
    f.setBold(true);
    p.setFont(f);
    p.drawText(rect(), Qt::AlignCenter,
               "🗂  드래그하여 추가\nDrag to add 3D files");
}

void DropOverlay::dragLeaveEvent(QDragLeaveEvent *e)
{
    hide();
    e->accept();
}
