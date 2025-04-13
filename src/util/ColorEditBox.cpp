#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QMouseEvent>

#include "ColorEditBox.h"

ColorEditBox::ColorEditBox(QWidget *parent) : QComboBox(parent) {
    this->setPlaceholderText("RGB(HEX)");
    this->setEditable(true);
    this->setEditText("#000000");
    this->setValidator(new QRegularExpressionValidator(
        QRegularExpression("#([A-Fa-f0-9]{6})"), this));
}

void ColorEditBox::setMenu(QMenu *menu) {
    popUpMenu = menu;
    connect(popUpMenu, &QMenu::aboutToHide, this, &ColorEditBox::hidePopupMenu);
}

void ColorEditBox::hidePopupMenu() { this->hidePopup(); }

void ColorEditBox::showPopup() {
    QPoint pos = this->mapToGlobal(QPoint(0, this->y() + this->height()));
    this->popUpMenu->popup(pos);
}

void ColorEditBox::hidePopup() {
    this->popUpMenu->hide();
    QComboBox::hidePopup();

    // To fix the expand button state not being reseted for no reason
    QMouseEvent fakeRelease(QEvent::MouseButtonRelease, this->pos(), this->pos(),
        Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &fakeRelease);
}
