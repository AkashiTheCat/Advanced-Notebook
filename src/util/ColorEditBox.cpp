#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QMouseEvent>
#include <QLineEdit>

#include "ColorEditBox.h"

ColorEditBox::ColorEditBox(QWidget *parent) : QComboBox(parent) {
    this->setPlaceholderText("RGB(HEX)");
    this->setEditable(true);
    this->setInsertPolicy(QComboBox::NoInsert);
    this->setEditText("#000000");
    this->lineEdit()->setMaxLength(7);

    connect(this->lineEdit(), &QLineEdit::editingFinished, this, [this]() {
        QLineEdit *le = this->lineEdit();
        if (!this->regex.match(le->text()).hasMatch())
            le->setText("#000000");
    });
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
