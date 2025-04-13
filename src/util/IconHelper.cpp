#include <QPainter>

#include "IconHelper.h"

void IconHelper::setIconColor(QIcon &icon, const QColor &color) {
    QPixmap pixmap = icon.pixmap(QSize(64,64));
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    icon.addPixmap(pixmap);
}
