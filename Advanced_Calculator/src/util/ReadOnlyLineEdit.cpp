#include "ReadOnlyLineEdit.h"
#include "QMenu"

ReadOnlyLineEdit::ReadOnlyLineEdit(QWidget *parent) : QLineEdit(parent) {
    setDragEnabled(false);
}

void ReadOnlyLineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        copy();
        return;
    }

    event->ignore();
}

void ReadOnlyLineEdit::contextMenuEvent(QContextMenuEvent *event) {
    QMenu *menu = new QMenu(this);
    QAction *copyAction = menu->addAction(tr("Copy"));
    copyAction->setEnabled(!this->text().isEmpty());

    connect(copyAction, &QAction::triggered, [this]() { copy(); });

    menu->exec(event->globalPos());
    delete menu;
}

void ReadOnlyLineEdit::dragEnterEvent(QDragEnterEvent *event) {
    event->ignore();
}

void ReadOnlyLineEdit::dropEvent(QDropEvent *event) {
    event->ignore();
}
